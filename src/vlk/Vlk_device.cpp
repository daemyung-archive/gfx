//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#define VMA_IMPLEMENTATION

#include <metrohash.h>
#include "std_lib.h"
#include "vlk_lib.h"
#include "Vlk_device.h"
#include "Vlk_buffer.h"
#include "Vlk_image.h"
#include "Vlk_sampler.h"
#include "Vlk_shader.h"
#include "Vlk_pipeline.h"
#include "Vlk_swap_chain.h"
#include "Vlk_cmd_buffer.h"
#include "Vlk_fence.h"
#include "Vlk_render_pass.h"
#include "Vlk_framebuffer.h"

using namespace std;
using namespace Platform_lib;
using namespace Gfx_lib;

//----------------------------------------------------------------------------------------------------------------------

#define DEFINE_VLK_SYMBOL(name) PFN_##name name;
#define LOAD_VLK_BOOTSTRAP_SYMBOL(name) name = library_.symbol<PFN_##name>(#name);
#define LOAD_VLK_INSTANCE_SYMBOL(name) name = (PFN_##name)vkGetInstanceProcAddr(instance_, #name);
#define LOAD_VLK_DEVICE_SYMBOL(name) name = (PFN_##name)vkGetDeviceProcAddr(device_, #name);

//----------------------------------------------------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------------------------------------------------

inline constexpr auto all_flags(uint32_t src, uint32_t test) noexcept
{
    return test == (src & test);
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

APPLY_VLK_BOOTSTRAP_SYMBOLS(DEFINE_VLK_SYMBOL)
APPLY_VLK_INSTANCE_CORE_SYMBOLS(DEFINE_VLK_SYMBOL)
APPLY_VLK_INSTANCE_SURFACE_SYMBOLS(DEFINE_VLK_SYMBOL)
APPLY_VLK_INSTANCE_ANDROID_SURFACE_SYMBOLS(DEFINE_VLK_SYMBOL)
APPLY_VLK_INSTANCE_WIN32_SURFACE_SYMBOLS(DEFINE_VLK_SYMBOL)
APPLY_VLK_INSTANCE_OSX_SURFACE_SYMBOLS(DEFINE_VLK_SYMBOL)
APPLY_VLK_INSTANCE_DEBUG_REPORT_SYMBOLS(DEFINE_VLK_SYMBOL)
APPLY_VLK_DEVICE_CORE_SYMBOLS(DEFINE_VLK_SYMBOL)
APPLY_VLK_DEVICE_SWAPCHAIN_SYMBOLS(DEFINE_VLK_SYMBOL)

//----------------------------------------------------------------------------------------------------------------------

Vlk_device::Vlk_device() :
    Device(),
    library_ {},
    instance_ { VK_NULL_HANDLE },
    physical_device_ { VK_NULL_HANDLE },
    queue_family_index_ { UINT32_MAX },
    device_ { VK_NULL_HANDLE },
    queue_ { VK_NULL_HANDLE },
    allocator_ { VK_NULL_HANDLE },
    command_pool_ { VK_NULL_HANDLE },
    render_pass_pool_ {},
    framebuffer_pool_ {}
{
    init_library_();
    init_bootstrap_symbols_();
    init_instance_();
    init_instance_symbols_();
    init_physical_device_();
    init_queue_family_index_();
    init_device_();
    init_device_symbols_();
    init_queue_();
    init_allocator_();
    init_command_pool_();
}

//----------------------------------------------------------------------------------------------------------------------

Vlk_device::~Vlk_device()
{
    framebuffer_pool_.clear();
    render_pass_pool_.clear();

    fini_command_pool_();
    fini_allocator_();
    fini_device_();
    fini_instance_();
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Buffer> Vlk_device::create(const Buffer_desc& desc)
{
    return make_unique<Vlk_buffer>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Image> Vlk_device::create(const Image_desc& desc)
{
    return make_unique<Vlk_image>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Sampler> Vlk_device::create(const Sampler_desc& desc)
{
    return make_unique<Vlk_sampler>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Shader> Vlk_device::create(const Shader_desc& desc)
{
    return make_unique<Vlk_shader>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Pipeline> Vlk_device::create(const Pipeline_desc& desc)
{
    return make_unique<Vlk_pipeline>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Swap_chain> Vlk_device::create(const Swap_chain_desc& desc)
{
    return make_unique<Vlk_swap_chain>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Cmd_buffer> Vlk_device::create(const Cmd_buffer_desc& desc)
{
    return make_unique<Vlk_cmd_buffer>(this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Fence> Vlk_device::create(const Fence_desc& desc)
{
    return make_unique<Vlk_fence>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_device::submit(Cmd_buffer* cmd_buffer, Fence* fence)
{
    // cast to the implementation.
    auto cmd_buffer_impl = static_cast<Vlk_cmd_buffer*>(cmd_buffer);
    auto fence_impl = static_cast<Vlk_fence*>(fence);

    // configure a submit info.
    VkSubmitInfo submit_info {};

    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd_buffer_impl->command_buffer();

    // submit a command buffer.
    vkQueueSubmit(queue_, 1, &submit_info, fence_impl ? fence_impl->fence() : VK_NULL_HANDLE);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_device::wait_idle()
{
    vkDeviceWaitIdle(device_);
}

//----------------------------------------------------------------------------------------------------------------------

Vlk_render_pass* Vlk_device::render_pass(const Render_encoder_desc& render_pass)
{
    // configure a render pass desc.
    Vlk_render_pass_desc desc {};

    for (auto i = 0; i != 4; ++i) {
        auto& color = render_pass.colors[i];

        if (!color.image)
            continue;

        desc.colors[i].format = color.image->format();
        desc.colors[i].samples = color.image->samples();
        desc.colors[i].load_op = color.load_op;
        desc.colors[i].store_op = color.store_op;
    }

    auto& depth_stencil = render_pass.depth_stencil;

    if (depth_stencil.image) {
        desc.depth_stencil.format = depth_stencil.image->format();
        desc.depth_stencil.samples = depth_stencil.image->samples();
        desc.depth_stencil.load_op = depth_stencil.load_op;
        desc.depth_stencil.store_op = depth_stencil.store_op;
    }

    // calculate a hash value.
    uint64_t key { 0 };

    MetroHash64::Hash(reinterpret_cast<uint8_t*>(&desc), sizeof(Vlk_render_pass_desc),
                      reinterpret_cast<uint8_t*>(&key));

    // check a render pass exists and if not then create it.
    if (!render_pass_pool_.contains(key))
        render_pass_pool_.emplace(key, desc, this);

    return *render_pass_pool_.find(key);
}

//----------------------------------------------------------------------------------------------------------------------

Vlk_render_pass* Vlk_device::render_pass(const Output_merger& output_merger)
{
    // configure a render pass desc.
    Vlk_render_pass_desc desc {};

    for (auto i = 0; i != 4; ++i) {
        desc.colors[i].format = output_merger.color_formats[i];
        desc.colors[i].samples = 1;
    }

    desc.depth_stencil.format = output_merger.depth_stencil_format;
    desc.depth_stencil.samples = 1;

    // calculate a hash value.
    uint64_t key { 0 };

    MetroHash64::Hash(reinterpret_cast<uint8_t*>(&desc), sizeof(Vlk_render_pass_desc),
                      reinterpret_cast<uint8_t*>(&key));

    // check a render pass exists and if not then create it.
    if (!render_pass_pool_.contains(key))
        render_pass_pool_.emplace(key, desc, this);

    return *render_pass_pool_.find(key);
}

//----------------------------------------------------------------------------------------------------------------------

Vlk_framebuffer* Vlk_device::framebuffer(const Render_encoder_desc& render_pass)
{
    // configure a framebuffer desc.
    Vlk_framebuffer_desc desc {};

    desc.render_pass = this->render_pass(render_pass);

    for (auto i = 0; i != 4; ++i) {
        // cast to the implementation.
        auto image_impl = static_cast<Vlk_image*>(render_pass.colors[i].image);

        if (!image_impl)
            continue;

        desc.images[i] = image_impl;
    }

    // cast to the implementation.
    desc.images[4] = static_cast<Vlk_image*>(render_pass.depth_stencil.image);

    // calculate a hash value.
    uint64_t key { 0 };

    MetroHash64::Hash(reinterpret_cast<uint8_t*>(&desc), sizeof(Vlk_framebuffer_desc),
                      reinterpret_cast<uint8_t*>(&key));

    // check a framebuffer exists and if not then create it.
    if (!framebuffer_pool_.contains(key))
        framebuffer_pool_.emplace(key, desc, this);

    return *framebuffer_pool_.find(key);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_device::init_library_()
{
    try {
#if defined(__ANDROID__)
        library_ = Library { "libvulkan.so" };
#elif defined(_WIN32)
        library_ = Library { "vulkan-1.dll" };
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
        library_ = Library { "libvulkan.dylib" };
#endif
    }
    catch (exception& e) {
        throw runtime_error("fail to create a device");
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_device::init_bootstrap_symbols_()
{
    APPLY_VLK_BOOTSTRAP_SYMBOLS(LOAD_VLK_BOOTSTRAP_SYMBOL)
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_device::init_instance_()
{
    vector<const char*> extensions {
#if defined(_DEBUG) || !defined(NDEBUG)
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif
        VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
        VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
        VK_MVK_MACOS_SURFACE_EXTENSION_NAME
#endif
    };

    // configure the application info.
    VkApplicationInfo app_info {};

    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.apiVersion = VK_MAKE_VERSION(1, 0, 0);

    // configure the instance create info.
    VkInstanceCreateInfo create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = extensions.size();
    create_info.ppEnabledExtensionNames = &extensions[0];

    // try to create an instance.
    if (vkCreateInstance(&create_info, nullptr, &instance_))
        throw runtime_error("fail to create a device");
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_device::init_instance_symbols_()
{
    APPLY_VLK_INSTANCE_CORE_SYMBOLS(LOAD_VLK_INSTANCE_SYMBOL)
    APPLY_VLK_INSTANCE_SURFACE_SYMBOLS(LOAD_VLK_INSTANCE_SYMBOL)
    APPLY_VLK_INSTANCE_ANDROID_SURFACE_SYMBOLS(LOAD_VLK_INSTANCE_SYMBOL)
    APPLY_VLK_INSTANCE_WIN32_SURFACE_SYMBOLS(LOAD_VLK_INSTANCE_SYMBOL)
    APPLY_VLK_INSTANCE_OSX_SURFACE_SYMBOLS(LOAD_VLK_INSTANCE_SYMBOL)
    APPLY_VLK_INSTANCE_DEBUG_REPORT_SYMBOLS(LOAD_VLK_INSTANCE_SYMBOL)
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_device::init_physical_device_()
{
    // query the physical device count.
    uint32_t count;

    vkEnumeratePhysicalDevices(instance_, &count, nullptr);

    if (!count)
        throw runtime_error("fail to create a device");

    // query physical devices
    vector<VkPhysicalDevice> physical_devices(count);

    vkEnumeratePhysicalDevices(instance_, &count, &physical_devices[0]);

    // todo: find the best physical device.
    physical_device_ = physical_devices[0];
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_device::init_queue_family_index_()
{
    // query the physical device queue family properties count.
    uint32_t count;

    vkGetPhysicalDeviceQueueFamilyProperties(physical_device_, &count, nullptr);

    // query the physical device queue family properties.
    assert(count);
    std::vector<VkQueueFamilyProperties> properties(count);

    vkGetPhysicalDeviceQueueFamilyProperties(physical_device_, &count, &properties[0]);

    // query the queue family index supporting the graphics and the compute.
    constexpr auto queue_flags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;

    for (auto i = 0; i != properties.size(); ++i) {
        if (all_flags(properties[i].queueFlags, queue_flags)) {
            queue_family_index_ = i;
            break;
        }
    }

    if (UINT32_MAX == queue_family_index_)
        throw runtime_error("fail to create a deivce");
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_device::init_device_()
{
    vector<const char*> extensions {
        "VK_KHR_swapchain",
        "VK_KHR_maintenance1"
    };

    // configure the device create info.
    VkDeviceQueueCreateInfo queue_create_info {};
    constexpr auto queue_priority { 0.0f };

    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = queue_family_index_;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;

    // configure the device create info.
    VkDeviceCreateInfo create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = 1;
    create_info.pQueueCreateInfos = &queue_create_info;
    create_info.enabledExtensionCount = extensions.size();
    create_info.ppEnabledExtensionNames = &extensions[0];

    // try to create a device
    if (vkCreateDevice(physical_device_, &create_info, nullptr, &device_))
        throw runtime_error("fail to create a deivce");
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_device::init_device_symbols_()
{
    APPLY_VLK_DEVICE_CORE_SYMBOLS(LOAD_VLK_DEVICE_SYMBOL)
    APPLY_VLK_DEVICE_SWAPCHAIN_SYMBOLS(LOAD_VLK_DEVICE_SYMBOL)
}

void Vlk_device::init_queue_()
{
    vkGetDeviceQueue(device_, queue_family_index_, 0, &queue_);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_device::init_allocator_()
{
    // set vulkan functions.
    VmaVulkanFunctions functions {};

    functions.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
    functions.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
    functions.vkAllocateMemory = vkAllocateMemory;
    functions.vkFreeMemory = vkFreeMemory;
    functions.vkMapMemory = vkMapMemory;
    functions.vkUnmapMemory = vkUnmapMemory;
    functions.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
    functions.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
    functions.vkBindBufferMemory = vkBindBufferMemory;
    functions.vkBindImageMemory = vkBindImageMemory;
    functions.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
    functions.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
    functions.vkCreateBuffer = vkCreateBuffer;
    functions.vkDestroyBuffer = vkDestroyBuffer;
    functions.vkCreateImage = vkCreateImage;
    functions.vkDestroyImage = vkDestroyImage;
    functions.vkCmdCopyBuffer = vkCmdCopyBuffer;

    // configure an allocator create info.
    VmaAllocatorCreateInfo create_info {};

    create_info.physicalDevice = physical_device_;
    create_info.device = device_;
    create_info.pVulkanFunctions = &functions;
    create_info.instance = instance_;
    create_info.vulkanApiVersion = VK_MAKE_VERSION(1, 0, 0);

    // try to create an allocator.
    if (vmaCreateAllocator(&create_info, &allocator_))
        throw runtime_error("fail to create a deivce");
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_device::init_command_pool_()
{
    // configure the command pool create info.
    VkCommandPoolCreateInfo create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    create_info.queueFamilyIndex = queue_family_index_;

    // try to create a command pool.
    if (vkCreateCommandPool(device_, &create_info, nullptr, &command_pool_))
        throw runtime_error("fail to create a device");
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_device::fini_instance_()
{
    vkDestroyInstance(instance_, nullptr);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_device::fini_device_()
{
    vkDestroyDevice(device_, nullptr);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_device::fini_allocator_()
{
    vmaDestroyAllocator(allocator_);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_device::fini_command_pool_()
{
    vkDestroyCommandPool(device_, command_pool_, nullptr);
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
