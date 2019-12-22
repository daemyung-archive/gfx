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

using namespace std;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Vlk_swap_chain::Vlk_swap_chain(const Swap_chain_desc& desc, Vlk_device* device) :
    Swap_chain(),
    device_ { device },
    image_format_ { desc.image_format },
    image_extent_ { desc.image_extent },
    color_space_ { desc.color_space },
    present_mode_ { desc.present_mode },
    frame_count_ { 0 },
    window_ { desc.window },
    surface_ { VK_NULL_HANDLE },
    swapchain_ { VK_NULL_HANDLE },
    images_ { desc.image_count },
    image_index_ { UINT32_MAX },
    cmd_buffers_ { desc.image_count },
    fences_ { desc.image_count },
    acquire_semaphores_ { desc.image_count },
    submit_semaphores_ { desc.image_count },
    frame_index_ { 0 }
{
    init_surface_(desc);
    init_swapchain_(desc);
    init_images_();
    init_cmd_buffers_();
    init_fences_();
    init_semaphores_();
}

//----------------------------------------------------------------------------------------------------------------------

Vlk_swap_chain::~Vlk_swap_chain()
{
    fini_semaphores_();
    fini_swapchain_();
    fini_surface_();
}

//----------------------------------------------------------------------------------------------------------------------

Image* Vlk_swap_chain::acquire()
{
    if (UINT32_MAX == image_index_) {
        vkAcquireNextImageKHR(device_->device(), swapchain_,
                              UINT64_MAX, cur_acquire_semaphore_(), VK_NULL_HANDLE,
                              &image_index_);
    }

    return images_[image_index_].get();
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_swap_chain::present()
{
    // query a fence is signaled or not, if not wait a fence to signal.
    if (!cur_fence_()->signaled())
        cur_fence_()->wait_signal();

    // reset a fence and a command buffer.
    cur_fence_()->reset();
    cur_cmd_buffer_()->reset();
    cur_cmd_buffer_()->start();

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
    cur_cmd_buffer_()->stop();

    // configure a wait destination stage mask.
    VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    // configure a submit info.
    VkSubmitInfo submit_info {};

    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &cur_acquire_semaphore_();
    submit_info.pWaitDstStageMask = &wait_dst_stage_mask;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cur_cmd_buffer_()->command_buffer();
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &cur_submit_semaphore_();

    // submit a command buffer.
    vkQueueSubmit(device_->queue(), 1, &submit_info, cur_fence_()->fence());

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

Format Vlk_swap_chain::image_format() const
{
    return image_format_;
}

//----------------------------------------------------------------------------------------------------------------------

Extent Vlk_swap_chain::image_extent() const
{
    return image_extent_;
}

//----------------------------------------------------------------------------------------------------------------------

Color_space Vlk_swap_chain::color_space() const
{
    return color_space_;
}

//----------------------------------------------------------------------------------------------------------------------

Present_mode Vlk_swap_chain::present_mode() const
{
    return present_mode_;
}

//----------------------------------------------------------------------------------------------------------------------

uint64_t Vlk_swap_chain::frame_count() const
{
    return frame_count_;
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_swap_chain::init_surface_(const Swap_chain_desc& desc)
{
#if VK_USE_PLATFORM_ANDROID_KHR
    auto window = static_cast<ANativeWindow*>(desc.window);

    // configure the surface create info.
    assert(window);
    VkAndroidSurfaceCreateInfoKHR create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    create_info.window = window;

    if (vkCreateAndroidSurfaceKHR(device_->instance(), &create_info, nullptr, &surface_))
        throw runtime_error("fail to create a swap chain");
#elif VK_USE_PLATFORM_WIN32_KHR
    auto window = static_cast<HWND>(desc.window);

    assert(window);
    VkWin32SurfaceCreateInfoKHR create_info{};

    create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    create_info.hinstance = GetModuleHandle(NULL);
    create_info.hwnd = window;

    if (vkCreateWin32SurfaceKHR(device_->instance(), &create_info, nullptr, &surface_))
        throw runtime_error("fail to create a swap chain");
#endif

    // query the physical device supports the surface.
    VkBool32 supported;

    vkGetPhysicalDeviceSurfaceSupportKHR(device_->physical_device(), device_->queue_family_index(),
                                         surface_, &supported);
    assert(supported);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_swap_chain::init_swapchain_(const Swap_chain_desc& desc)
{
    // query the surface capabilities.
    VkSurfaceCapabilitiesKHR surface_caps;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device_->physical_device(), surface_, &surface_caps);

    // configure a swapchain create info.
    VkSwapchainCreateInfoKHR create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface_;
    create_info.minImageCount = desc.image_count;
    create_info.imageFormat = convert<VkFormat>(desc.image_format);
    create_info.imageColorSpace = convert<VkColorSpaceKHR>(desc.color_space);
    create_info.imageExtent = surface_caps.currentExtent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    create_info.presentMode = convert<VkPresentModeKHR>(desc.present_mode);
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

void Vlk_swap_chain::init_cmd_buffers_()
{
    // try to create command buffers.
    for (auto& cmd_buffer : cmd_buffers_)
        cmd_buffer = make_unique<Vlk_cmd_buffer>(device_);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_swap_chain::init_fences_()
{
    // configure a fence descriptor.
    Fence_desc desc;

    desc.signaled = true;

    // try to create fences.
    for (auto& fence : fences_)
        fence = make_unique<Vlk_fence>(desc, device_);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_swap_chain::init_semaphores_()
{
    // configure a semaphore create info.
    VkSemaphoreCreateInfo create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    // try to create acquire semaphores.
    for (auto& semaphore : acquire_semaphores_) {
        if (vkCreateSemaphore(device_->device(), &create_info, nullptr, &semaphore))
            throw runtime_error("fail to create a swap chain");
    }

    // try to create submit semaphores.
    for (auto& semaphore : submit_semaphores_) {
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

void Vlk_swap_chain::fini_semaphores_()
{
    for (auto& semaphore : acquire_semaphores_) {
        vkDestroySemaphore(device_->device(), semaphore, nullptr);
    }

    for (auto& semaphore : submit_semaphores_) {
        vkDestroySemaphore(device_->device(), semaphore, nullptr);
    }
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
