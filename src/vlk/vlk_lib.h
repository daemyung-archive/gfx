//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_VLK_LIB_MODULES_GUARD
#define GFX_VLK_LIB_MODULES_GUARD

#include <platform/build_target.h>

#define VK_NO_PROTOTYPES 1

#if defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR
#elif defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#elif TARGET_OS_OSX
#define VK_USE_PLATFORM_MACOS_MVK
#endif

#include <stdexcept>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include "gfx/enums.h"
#include "gfx/types.h"

//----------------------------------------------------------------------------------------------------------------------

#define APPLY_VLK_BOOTSTRAP_SYMBOLS(macro) \
    macro(vkGetInstanceProcAddr) \
    macro(vkCreateInstance) \
    macro(vkDestroyInstance) \
    macro(vkEnumerateInstanceExtensionProperties) \
    macro(vkEnumerateInstanceLayerProperties) \
    macro(vkEnumeratePhysicalDevices) \
    macro(vkGetPhysicalDeviceProperties )

#define APPLY_VLK_INSTANCE_CORE_SYMBOLS(macro) \
    macro(vkGetDeviceProcAddr) \
    macro(vkGetPhysicalDeviceFeatures) \
    macro(vkGetPhysicalDeviceFormatProperties) \
    macro(vkGetPhysicalDeviceImageFormatProperties) \
    macro(vkGetPhysicalDeviceQueueFamilyProperties) \
    macro(vkGetPhysicalDeviceMemoryProperties) \
    macro(vkGetPhysicalDeviceSparseImageFormatProperties) \
    macro(vkCreateDevice) \
    macro(vkDestroyDevice) \
    macro(vkEnumerateDeviceExtensionProperties) \
    macro(vkEnumerateDeviceLayerProperties)

#define APPLY_VLK_INSTANCE_SURFACE_SYMBOLS(macro) \
    macro(vkDestroySurfaceKHR) \
    macro(vkGetPhysicalDeviceSurfaceSupportKHR) \
    macro(vkGetPhysicalDeviceSurfaceCapabilitiesKHR) \
    macro(vkGetPhysicalDeviceSurfaceFormatsKHR) \
    macro(vkGetPhysicalDeviceSurfacePresentModesKHR)

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#define APPLY_VLK_INSTANCE_ANDROID_SURFACE_SYMBOLS(macro) \
    macro(vkCreateAndroidSurfaceKHR)
#else
#define APPLY_VLK_INSTANCE_ANDROID_SURFACE_SYMBOLS(macro)
#endif

#if defined(VK_USE_PLATFORM_WIN32_KHR)
#define APPLY_VLK_INSTANCE_WIN32_SURFACE_SYMBOLS(macro) \
    macro(vkCreateWin32SurfaceKHR) \
    macro(vkGetPhysicalDeviceWin32PresentationSupportKHR)
#else
#define APPLY_VLK_INSTANCE_WIN32_SURFACE_SYMBOLS(macro)
#endif

#if defined(VK_USE_PLATFORM_MACOS_MVK)
#define APPLY_VLK_INSTANCE_OSX_SURFACE_SYMBOLS(macro) \
    macro(vkCreateMacOSSurfaceMVK)
#else
#define APPLY_VLK_INSTANCE_OSX_SURFACE_SYMBOLS(macro)
#endif

#define APPLY_VLK_INSTANCE_DEBUG_REPORT_SYMBOLS(macro) \
    macro(vkCreateDebugReportCallbackEXT) \
    macro(vkDebugReportMessageEXT) \
    macro(vkDestroyDebugReportCallbackEXT)

#define APPLY_VLK_DEVICE_CORE_SYMBOLS(macro) \
    macro(vkGetDeviceQueue) \
    macro(vkQueueSubmit) \
    macro(vkQueueWaitIdle) \
    macro(vkDeviceWaitIdle) \
    macro(vkAllocateMemory) \
    macro(vkFreeMemory) \
    macro(vkMapMemory) \
    macro(vkUnmapMemory) \
    macro(vkFlushMappedMemoryRanges) \
    macro(vkInvalidateMappedMemoryRanges) \
    macro(vkGetDeviceMemoryCommitment) \
    macro(vkBindBufferMemory) \
    macro(vkBindImageMemory) \
    macro(vkGetBufferMemoryRequirements) \
    macro(vkGetImageMemoryRequirements) \
    macro(vkGetImageSparseMemoryRequirements) \
    macro(vkQueueBindSparse) \
    macro(vkCreateFence) \
    macro(vkDestroyFence) \
    macro(vkResetFences) \
    macro(vkGetFenceStatus) \
    macro(vkWaitForFences) \
    macro(vkCreateSemaphore) \
    macro(vkDestroySemaphore) \
    macro(vkCreateEvent) \
    macro(vkDestroyEvent) \
    macro(vkGetEventStatus) \
    macro(vkSetEvent) \
    macro(vkResetEvent) \
    macro(vkCreateQueryPool) \
    macro(vkDestroyQueryPool) \
    macro(vkGetQueryPoolResults) \
    macro(vkCreateBuffer) \
    macro(vkDestroyBuffer) \
    macro(vkCreateBufferView) \
    macro(vkDestroyBufferView) \
    macro(vkCreateImage) \
    macro(vkDestroyImage) \
    macro(vkGetImageSubresourceLayout) \
    macro(vkCreateImageView) \
    macro(vkDestroyImageView) \
    macro(vkCreateShaderModule) \
    macro(vkDestroyShaderModule) \
    macro(vkCreatePipelineCache) \
    macro(vkDestroyPipelineCache) \
    macro(vkGetPipelineCacheData) \
    macro(vkMergePipelineCaches) \
    macro(vkCreateGraphicsPipelines) \
    macro(vkCreateComputePipelines) \
    macro(vkDestroyPipeline) \
    macro(vkCreatePipelineLayout) \
    macro(vkDestroyPipelineLayout) \
    macro(vkCreateSampler) \
    macro(vkDestroySampler) \
    macro(vkCreateDescriptorSetLayout) \
    macro(vkDestroyDescriptorSetLayout) \
    macro(vkCreateDescriptorPool) \
    macro(vkDestroyDescriptorPool) \
    macro(vkResetDescriptorPool) \
    macro(vkAllocateDescriptorSets) \
    macro(vkFreeDescriptorSets) \
    macro(vkUpdateDescriptorSets) \
    macro(vkCreateFramebuffer) \
    macro(vkDestroyFramebuffer) \
    macro(vkCreateRenderPass) \
    macro(vkDestroyRenderPass) \
    macro(vkGetRenderAreaGranularity) \
    macro(vkCreateCommandPool) \
    macro(vkDestroyCommandPool) \
    macro(vkResetCommandPool) \
    macro(vkAllocateCommandBuffers) \
    macro(vkFreeCommandBuffers) \
    macro(vkBeginCommandBuffer) \
    macro(vkEndCommandBuffer) \
    macro(vkResetCommandBuffer) \
    macro(vkCmdBindPipeline) \
    macro(vkCmdSetViewport) \
    macro(vkCmdSetScissor) \
    macro(vkCmdSetLineWidth) \
    macro(vkCmdSetDepthBias) \
    macro(vkCmdSetBlendConstants) \
    macro(vkCmdSetDepthBounds) \
    macro(vkCmdSetStencilCompareMask) \
    macro(vkCmdSetStencilWriteMask) \
    macro(vkCmdSetStencilReference) \
    macro(vkCmdBindDescriptorSets) \
    macro(vkCmdBindIndexBuffer) \
    macro(vkCmdBindVertexBuffers) \
    macro(vkCmdDraw) \
    macro(vkCmdDrawIndexed) \
    macro(vkCmdDrawIndirect) \
    macro(vkCmdDrawIndexedIndirect) \
    macro(vkCmdDispatch) \
    macro(vkCmdDispatchIndirect) \
    macro(vkCmdCopyBuffer) \
    macro(vkCmdCopyImage) \
    macro(vkCmdBlitImage) \
    macro(vkCmdCopyBufferToImage) \
    macro(vkCmdCopyImageToBuffer) \
    macro(vkCmdUpdateBuffer) \
    macro(vkCmdFillBuffer) \
    macro(vkCmdClearColorImage) \
    macro(vkCmdClearDepthStencilImage) \
    macro(vkCmdClearAttachments) \
    macro(vkCmdResolveImage) \
    macro(vkCmdSetEvent) \
    macro(vkCmdResetEvent) \
    macro(vkCmdWaitEvents) \
    macro(vkCmdPipelineBarrier) \
    macro(vkCmdBeginQuery) \
    macro(vkCmdEndQuery) \
    macro(vkCmdResetQueryPool) \
    macro(vkCmdWriteTimestamp) \
    macro(vkCmdCopyQueryPoolResults) \
    macro(vkCmdPushConstants) \
    macro(vkCmdBeginRenderPass) \
    macro(vkCmdNextSubpass) \
    macro(vkCmdEndRenderPass) \
    macro(vkCmdExecuteCommands)

#define APPLY_VLK_DEVICE_SWAPCHAIN_SYMBOLS(macro) \
    macro(vkCreateSwapchainKHR) \
    macro(vkDestroySwapchainKHR) \
    macro(vkGetSwapchainImagesKHR) \
    macro(vkAcquireNextImageKHR) \
    macro(vkQueuePresentKHR)

#define DECLARE_VLK_SYMBOL(name) extern PFN_##name name;

//----------------------------------------------------------------------------------------------------------------------

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

APPLY_VLK_BOOTSTRAP_SYMBOLS(DECLARE_VLK_SYMBOL)
APPLY_VLK_INSTANCE_CORE_SYMBOLS(DECLARE_VLK_SYMBOL)
APPLY_VLK_INSTANCE_SURFACE_SYMBOLS(DECLARE_VLK_SYMBOL)
APPLY_VLK_INSTANCE_ANDROID_SURFACE_SYMBOLS(DECLARE_VLK_SYMBOL)
APPLY_VLK_INSTANCE_WIN32_SURFACE_SYMBOLS(DECLARE_VLK_SYMBOL)
APPLY_VLK_INSTANCE_OSX_SURFACE_SYMBOLS(DECLARE_VLK_SYMBOL)
APPLY_VLK_INSTANCE_DEBUG_REPORT_SYMBOLS(DECLARE_VLK_SYMBOL)
APPLY_VLK_DEVICE_CORE_SYMBOLS(DECLARE_VLK_SYMBOL)
APPLY_VLK_DEVICE_SWAPCHAIN_SYMBOLS(DECLARE_VLK_SYMBOL)

//----------------------------------------------------------------------------------------------------------------------

inline VkExtent2D to_VkExtent2D(Extent extent)
{
    return { extent.w, extent.h };
}

//----------------------------------------------------------------------------------------------------------------------

inline VkExtent3D to_VkExtent3D(Extent extent)
{
    return { extent.w, extent.h, extent.d };
}

//----------------------------------------------------------------------------------------------------------------------

inline VmaMemoryUsage to_VmaMemoryUsage(Heap_type type)
{
    switch (type) {
        case Heap_type::local:
            return VMA_MEMORY_USAGE_GPU_ONLY;
        case Heap_type::upload:
            return VMA_MEMORY_USAGE_CPU_TO_GPU;
        case Heap_type::readback:
            return VMA_MEMORY_USAGE_GPU_TO_CPU;
        default:
            throw std::runtime_error("invalid the heap type");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline VkImageType to_VkImageType(Image_type type)
{
    switch (type) {
        case Image_type::two_dim:
        case Image_type::cube:
            return VK_IMAGE_TYPE_2D;
        default:
            throw std::runtime_error("invalid the image type");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline VkFormat to_VkFormat(Format format)
{
    switch (format) {
        case Format::rgb8_unorm:
            return VK_FORMAT_R8G8B8_UNORM;
        case Format::rgba8_unorm:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case Format::bgra8_unorm:
            return VK_FORMAT_B8G8R8A8_UNORM;
        case Format::r32_float:
            return VK_FORMAT_R32_SFLOAT;
        case Format::rg32_float:
            return VK_FORMAT_R32G32_SFLOAT;
        case Format::rgb32_float:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case Format::rgba32_float:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case Format::d24_unorm_s8_uint:
            return VK_FORMAT_D24_UNORM_S8_UINT;
        default:
            throw std::runtime_error("invalid the format");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline VkImageAspectFlags to_VkImageAspectFlags(Format format)
{
    switch (format) {
        case Format::rgba8_unorm:
            return VK_IMAGE_ASPECT_COLOR_BIT;
        case Format::bgra8_unorm:
            return VK_IMAGE_ASPECT_COLOR_BIT;
        case Format::r32_float:
            return VK_IMAGE_ASPECT_COLOR_BIT;
        case Format::rg32_float:
            return VK_IMAGE_ASPECT_COLOR_BIT;
        case Format::rgba32_float:
            return VK_IMAGE_ASPECT_COLOR_BIT;
        case Format::d24_unorm_s8_uint:
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        default:
            throw std::runtime_error("invalid the format");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline VkSampleCountFlagBits to_VkSampleCountFlagBits(uint8_t samples)
{
    return static_cast<VkSampleCountFlagBits>(0x1 << (samples - 1));
}

//----------------------------------------------------------------------------------------------------------------------

inline VkImageViewType to_VkImageViewType(Image_type type)
{
    switch (type) {
        case Image_type::two_dim:
        case Image_type::swap_chain:
            return VK_IMAGE_VIEW_TYPE_2D;
        case Image_type::cube:
            return VK_IMAGE_VIEW_TYPE_CUBE;
        default:
            throw std::runtime_error("invalid the image type");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline VkFilter to_VkFilter(Filter filter)
{
    switch (filter) {
        case Filter::nearest:
            return VK_FILTER_NEAREST;
        case Filter::linear:
            return VK_FILTER_LINEAR;
        default:
            throw std::runtime_error("invalid the filter");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline VkSamplerMipmapMode to_VkSamplerMipmapMode(Mip_filter filter)
{
    switch (filter) {
        case Mip_filter::nearest:
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case Mip_filter::linear:
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        default:
            throw std::runtime_error("invalid the filter");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline VkSamplerAddressMode to_VkSamplerAddressMode(Address_mode mode)
{
    switch (mode) {
        case Address_mode::repeat:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case Address_mode::clamp_to_edge:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        default:
            throw std::runtime_error("invalid the address mode");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline VkShaderStageFlagBits to_VkShaderStageFlagBits(Sc_lib::Shader_type type)
{
    switch (type) {
        case Sc_lib::Shader_type::vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case Sc_lib::Shader_type ::fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        default:
            throw std::runtime_error("invalid the shader type");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline VkVertexInputRate to_VkVertexInputRate(Step_rate rate)
{
    switch (rate) {
        case Step_rate::vertex:
            return VK_VERTEX_INPUT_RATE_VERTEX;
        case Step_rate::instance:
            return VK_VERTEX_INPUT_RATE_INSTANCE;
        default:
            throw std::runtime_error("invalid the step rate");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline VkPrimitiveTopology to_VkPrimitiveTopology(Topology topology)
{
    switch (topology) {
        case Topology::triangle_list:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case Topology::triangle_strip:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        case Topology::point:
            return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        default:
            throw std::runtime_error("invalid the topology");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline VkCullModeFlags to_VkCullModeFlags(Cull_mode mode)
{
    switch (mode) {
        case Cull_mode::front:
            return VK_CULL_MODE_FRONT_BIT;
        case Cull_mode::back:
            return VK_CULL_MODE_BACK_BIT;
        case Cull_mode::none:
            return VK_CULL_MODE_NONE;
        default:
            throw std::runtime_error("invalid the cull mode");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline VkFrontFace to_VkFrontFace(Front_face face)
{
    switch (face) {
        case Front_face::counter_clockwise:
            return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        case Front_face::clockwise:
            return VK_FRONT_FACE_CLOCKWISE;
        default:
            throw std::runtime_error("invalid the front face");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline VkCompareOp to_VkCompareOp(Compare_op op)
{
    switch (op) {
        case Compare_op::never:
            return VK_COMPARE_OP_NEVER;
        case Compare_op::less:
            return VK_COMPARE_OP_LESS;
        case Compare_op::greater:
            return VK_COMPARE_OP_GREATER;
        case Compare_op::equal:
            return VK_COMPARE_OP_EQUAL;
        case Compare_op::not_equal:
            return VK_COMPARE_OP_NOT_EQUAL;
        case Compare_op::less_or_equal:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case Compare_op::greater_or_equal:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case Compare_op::always:
            return VK_COMPARE_OP_ALWAYS;
        default:
            throw std::runtime_error("invalid the compare op");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline VkStencilOp to_VkStencilOp(Stencil_op op)
{
    switch (op) {
        case Stencil_op::keep:
            return VK_STENCIL_OP_KEEP;
        case Stencil_op::zero:
            return VK_STENCIL_OP_ZERO;
        case Stencil_op::replace:
            return VK_STENCIL_OP_REPLACE;
        case Stencil_op::increment_and_clamp:
            return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        case Stencil_op::decrement_and_clamp:
            return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        case Stencil_op::invert:
            return VK_STENCIL_OP_INVERT;
        case Stencil_op::increment_and_wrap:
            return VK_STENCIL_OP_INCREMENT_AND_WRAP;
        case Stencil_op::decrement_and_wrap:
            return VK_STENCIL_OP_DECREMENT_AND_WRAP;
        default:
            throw std::runtime_error("invalid the stencil op");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline VkBlendFactor to_VkBlendFactor(Blend_factor factor)
{
    switch (factor) {
        case Blend_factor::zero:
            return VK_BLEND_FACTOR_ZERO;
        case Blend_factor::one:
            return VK_BLEND_FACTOR_ONE;
        case Blend_factor::src_alpha:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case Blend_factor::one_minus_src_alpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case Blend_factor::dst_alpha:
            return VK_BLEND_FACTOR_DST_ALPHA;
        case Blend_factor::one_minus_dst_alpha:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        default:
            throw std::runtime_error("invalid the blend factor");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline VkBlendOp to_VkBlendOp(Blend_op op)
{
    switch (op) {
        case Blend_op::add:
            return VK_BLEND_OP_ADD;
        case Blend_op::subtract:
            return VK_BLEND_OP_SUBTRACT;
        case Blend_op::reverse_subtract:
            return VK_BLEND_OP_REVERSE_SUBTRACT;
        case Blend_op::min:
            return VK_BLEND_OP_MIN;
        case Blend_op::max:
            return VK_BLEND_OP_MAX;
        default:
            throw std::runtime_error("invalid the blend op");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline VkIndexType to_VkIndexType(Index_type type)
{
    switch (type) {
        case Index_type::uint16:
            return VK_INDEX_TYPE_UINT16;
        case Index_type::uint32:
            return VK_INDEX_TYPE_UINT32;
        default:
            throw std::runtime_error("invalid the index type");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline VkViewport to_VkViewport(Viewport viewport)
{
    return {viewport.x, viewport.h - viewport.y, viewport.w, -viewport.h, 0.0f, 1.0f};
}

//----------------------------------------------------------------------------------------------------------------------

inline VkRect2D to_VkRect2D(Scissor scissor)
{
    return {{static_cast<int32_t>(scissor.x), static_cast<int32_t>(scissor.y)}, {scissor.w, scissor.h}};
}

//----------------------------------------------------------------------------------------------------------------------

inline VkAttachmentLoadOp to_VkAttachmentLoadOp(Load_op op)
{
    switch (op) {
        case Load_op::load:
            return VK_ATTACHMENT_LOAD_OP_LOAD;
        case Load_op::clear:
            return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case Load_op::dont_care:
            return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        default:
            throw std::runtime_error("invalid the load op");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline VkAttachmentStoreOp to_VkAttachmentStoreOp(Store_op op)
{
    switch (op) {
        case Store_op::store:
            return VK_ATTACHMENT_STORE_OP_STORE;
        case Store_op::dont_care:
            return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        default:
            throw std::runtime_error("invalid the store op");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline VkColorSpaceKHR to_VkColorSpaceKHR(Color_space space)
{
    switch (space) {
        case Color_space::srgb_non_linear:
            return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        default:
            throw std::runtime_error("invalid color space");
    }
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_VLK_LIB_MODULES_GUARD
