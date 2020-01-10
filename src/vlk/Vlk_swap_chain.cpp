//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include <platform/Window.h>
#include "std_lib.h"
#include "vlk_lib.h"
#include "Vlk_swap_chain.h"
#include "Vlk_device.h"
#include "Vlk_image.h"
#include "Vlk_cmd_buffer.h"
#include "Vlk_fence.h"

#if TARGET_OS_OSX
#include <QuartzCore/CAMetalLayer.h>
#include "mtl_lib.h"
#endif

using namespace std;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Vlk_swap_chain::Vlk_swap_chain(const Swap_chain_desc& desc, Vlk_device* device) :
    Swap_chain {desc},
    device_ {device},
    surface_ {VK_NULL_HANDLE},
    swapchain_ {VK_NULL_HANDLE},
    images_ {desc.image_count},
    image_index_ {UINT32_MAX},
    acquire_fence_ {nullptr},
    cmd_buffers_ {desc.image_count},
    submit_fences_ {desc.image_count},
    submit_semaphores_ {desc.image_count},
    frame_index_ {0}
{
    init_surface_(desc.window);
    init_swapchain_();
    init_images_();
    init_acquire_fence_();
    init_cmd_buffers_();
    init_submit_fences_();
    init_submit_semaphores_();
}

//----------------------------------------------------------------------------------------------------------------------

Vlk_swap_chain::~Vlk_swap_chain()
{
    fini_submit_semaphores_();
    fini_swapchain_();
    fini_surface_();
}

//----------------------------------------------------------------------------------------------------------------------

Image* Vlk_swap_chain::acquire()
{
    if (UINT32_MAX == image_index_) {
        acquire_fence_->reset();
        vkAcquireNextImageKHR(device_->device(), swapchain_,
                              UINT64_MAX, VK_NULL_HANDLE, acquire_fence_->fence(),
                              &image_index_);
        acquire_fence_->wait_signal();
    }

    return images_[image_index_].get();
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_swap_chain::present()
{
    // query a fence is signaled or not, if not wait a fence to signal.
    if (!cur_submit_fence_()->signaled())
        cur_submit_fence_()->wait_signal();

    // reset a fence and a command buffer.
    cur_submit_fence_()->reset();
    cur_cmd_buffer_()->reset();

    // configure an image barrier.
    VkImageMemoryBarrier image_barrier {};

    image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_barrier.srcAccessMask = cur_image_()->access_mask();
    image_barrier.dstAccessMask = 0;
    image_barrier.oldLayout = cur_image_()->layout();
    image_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    image_barrier.image = cur_image_()->image();
    image_barrier.subresourceRange.aspectMask = cur_image_()->aspect_mask();
    image_barrier.subresourceRange.levelCount = cur_image_()->mip_levels();
    image_barrier.subresourceRange.layerCount = cur_image_()->array_layers();

    // update image meta data.
    cur_image_()->access_mask_ = 0;
    cur_image_()->layout_ = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // record barrier command.
    vkCmdPipelineBarrier(cur_cmd_buffer_()->command_buffer(),
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                         VK_DEPENDENCY_BY_REGION_BIT,
                         0, nullptr,
                         0, nullptr,
                         1, &image_barrier);
    cur_cmd_buffer_()->end();

    // configure a submit info.
    VkSubmitInfo submit_info {};

    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cur_cmd_buffer_()->command_buffer();
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &cur_submit_semaphore_();

    // submit a command buffer.
    vkQueueSubmit(device_->queue(), 1, &submit_info, cur_submit_fence_()->fence());

    // configure a present info.
    VkPresentInfoKHR present_info {};

    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &cur_submit_semaphore_();
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain_;
    present_info.pImageIndices = &image_index_;

    // present.
    vkQueuePresentKHR(device_->queue(), &present_info);

    image_index_ = UINT32_MAX;
    frame_index_ = ++frame_count_ % images_.size();
}

//----------------------------------------------------------------------------------------------------------------------

Device* Vlk_swap_chain::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_swap_chain::init_surface_(void* window)
{
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    // configure an android surface create info.
    VkAndroidSurfaceCreateInfoKHR create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    create_info.window = static_cast<ANativeWindow*>(window);

    // try to create an android surface.
    if (vkCreateAndroidSurfaceKHR(device_->instance(), &create_info, nullptr, &surface_))
        throw runtime_error("fail to create a swap chain");
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
    // configure a win32 surface create info.
    VkWin32SurfaceCreateInfoKHR create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    create_info.hinstance = GetModuleHandle(NULL);
    create_info.hwnd = static_cast<HWND>(window);

    // try to create an win32 surface.
    if (vkCreateWin32SurfaceKHR(device_->instance(), &create_info, nullptr, &surface_))
        throw runtime_error("fail to create a swap chain");
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
    // create a metal layer.
    auto layer = [CAMetalLayer layer];

    if (!layer)
        throw runtime_error("fail to create a swap chain");

    // configure a metal layer.
    layer.pixelFormat = to_MTLPixelFormat(image_format_);
    layer.framebufferOnly = NO;
    layer.maximumDrawableCount = images_.size();

    // set a metal layer to a view.
    [[(__bridge NSWindow*)window contentView] setLayer:layer];

    // configure an osx surface create info.
    VkMacOSSurfaceCreateInfoMVK create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
    create_info.pView = (__bridge void*)[(__bridge NSWindow*)window contentView];

    // try to create an osx surface.
    if (vkCreateMacOSSurfaceMVK(device_->instance(), &create_info, nullptr, &surface_))
        throw runtime_error("fail to create a swap chain");
#endif

    // query the physical device supports the surface.
    VkBool32 supported;

    vkGetPhysicalDeviceSurfaceSupportKHR(device_->physical_device(), device_->queue_family_index(),
                                         surface_, &supported);
    assert(supported);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_swap_chain::init_swapchain_()
{
    // query the surface capabilities.
    VkSurfaceCapabilitiesKHR surface_caps;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device_->physical_device(), surface_, &surface_caps);

    // configure a swapchain create info.
    VkSwapchainCreateInfoKHR create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface_;
    create_info.minImageCount = images_.size();
    create_info.imageFormat = to_VkFormat(image_format_);
    create_info.imageColorSpace = to_VkColorSpaceKHR(color_space_);
    create_info.imageExtent = surface_caps.currentExtent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    create_info.clipped = VK_TRUE;

    // try to create a swapchain.
    if (vkCreateSwapchainKHR(device_->device(), &create_info, nullptr, &swapchain_))
        throw runtime_error("fail to create a swap chain");
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_swap_chain::init_images_()
{
    // query the swapchain image count.
    uint32_t count;

    vkGetSwapchainImagesKHR(device_->device(), swapchain_, &count, nullptr);

    // query the swapchain images.
    assert(count == images_.size());
    vector<VkImage> images(count);

    vkGetSwapchainImagesKHR(device_->device(), swapchain_, &count, &images[0]);

    // configure an image desc.
    Image_desc desc;

    desc.type = Image_type::swap_chain;
    desc.format = image_format_;
    desc.extent = image_extent_;

    // create images.
    for (auto i = 0; i != images_.size(); ++i) {
        try {
            images_[i] = make_unique<Vlk_image>(desc, device_, this, images[i]);
        }
        catch (exception& except) {
            throw runtime_error("fail to create a swap chain");
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_swap_chain::init_acquire_fence_()
{
    // configure a fence descriptor.
    Fence_desc desc;

    desc.signaled = true;

    // try to create fences.
    try {
        acquire_fence_ = make_unique<Vlk_fence>(desc, device_);
    }
    catch (exception& e) {
        throw runtime_error("fail to create a swap chain");
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_swap_chain::init_cmd_buffers_()
{
    // try to create command buffers.
    for (auto& cmd_buffer : cmd_buffers_)
        cmd_buffer = make_unique<Vlk_cmd_buffer>(device_);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_swap_chain::init_submit_fences_()
{
    // try to create fences.
    for (auto& fence : submit_fences_) {
        // configure a fence descriptor.
        Fence_desc desc;

        desc.signaled = true;

        try {
            fence = make_unique<Vlk_fence>(desc, device_);
        }
        catch (exception& e) {
            throw runtime_error("fail to create a swap chain");
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_swap_chain::init_submit_semaphores_()
{
    for (auto& semaphore : submit_semaphores_) {
        // configure a semaphore create info.
        VkSemaphoreCreateInfo create_info {};

        create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        // try to create submit semaphores.
        if (vkCreateSemaphore(device_->device(), &create_info, nullptr, &semaphore))
            throw runtime_error("fail to create a swap chain");
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_swap_chain::fini_surface_()
{
    vkDestroySurfaceKHR(device_->instance(), surface_, nullptr);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_swap_chain::fini_swapchain_()
{
    vkDestroySwapchainKHR(device_->device(), swapchain_, nullptr);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_swap_chain::fini_submit_semaphores_()
{
    for (auto& semaphore : submit_semaphores_) {
        vkDestroySemaphore(device_->device(), semaphore, nullptr);
    }
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
