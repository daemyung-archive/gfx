//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_VLK_LIB_MODULES_GUARD
#define GFX_VLK_LIB_MODULES_GUARD

#define VK_NO_PROTOTYPES 1

#if defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR 1
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
    macro(vkGetPhysicalDeviceSurfacePresentModesKHR) \
    macro(vkCreateAndroidSurfaceKHR)
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
    macro(vkCmdExecuteCommands )
#define APPLY_VLK_DEVICE_SWAPCHAIN_SYMBOLS(macro) \
    macro(vkCreateSwapchainKHR) \
    macro(vkDestroySwapchainKHR) \
    macro(vkGetSwapchainImagesKHR) \
    macro(vkAcquireNextImageKHR) \
    macro(vkQueuePresentKHR )
#define DECLARE_VLK_SYMBOL(name) extern PFN_##name name;

//----------------------------------------------------------------------------------------------------------------------

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

APPLY_VLK_BOOTSTRAP_SYMBOLS(DECLARE_VLK_SYMBOL)
APPLY_VLK_INSTANCE_CORE_SYMBOLS(DECLARE_VLK_SYMBOL)
APPLY_VLK_INSTANCE_SURFACE_SYMBOLS(DECLARE_VLK_SYMBOL)
APPLY_VLK_INSTANCE_DEBUG_REPORT_SYMBOLS(DECLARE_VLK_SYMBOL)
APPLY_VLK_DEVICE_CORE_SYMBOLS(DECLARE_VLK_SYMBOL)
APPLY_VLK_DEVICE_SWAPCHAIN_SYMBOLS(DECLARE_VLK_SYMBOL)

//----------------------------------------------------------------------------------------------------------------------

template<typename T>
inline T convert(const Extent&);

template<typename T>
inline T convert(Image_type);

template<typename T>
inline T convert(Format);

//----------------------------------------------------------------------------------------------------------------------

template <>
inline VkExtent2D convert(const Extent& extent)
{
    return { extent.w, extent.h };
}

//----------------------------------------------------------------------------------------------------------------------

template <>
inline VkExtent3D convert(const Extent& extent)
{
    return { extent.w, extent.h, extent.d };
}

//----------------------------------------------------------------------------------------------------------------------

inline auto convert(Heap_type type)
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

template <>
inline VkImageType convert(Image_type type)
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

template<>
inline VkFormat convert(Format format)
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

template<>
inline VkImageAspectFlags convert(Format format)
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

inline auto convert(uint32_t samples)
{
    return static_cast<VkSampleCountFlagBits>(0x1 << (samples - 1));
}

//----------------------------------------------------------------------------------------------------------------------

template <>
inline VkImageViewType convert(Image_type type)
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

inline auto convert(Filter filter)
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

inline auto convert(Mip_filter filter)
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

inline auto convert(Address_mode mode)
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

inline auto convert(Step_rate rate)
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

inline auto convert(Topology topology)
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

inline auto convert(Cull_mode mode)
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

inline auto convert(Front_face face)
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

inline auto convert(Compare_op op)
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
            throw std::runtime_error("invalid the stencil op");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline auto convert(Blend_factor factor)
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

inline auto convert(Blend_op op)
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

inline auto convert(Index_type type)
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

inline auto convert(Pipeline_type type)
{
    switch (type) {
        case Pipeline_type::render:
            return VK_PIPELINE_BIND_POINT_GRAPHICS;
        case Pipeline_type::compute:
            return VK_PIPELINE_BIND_POINT_COMPUTE;
        default:
            throw std::runtime_error("invalid the pipeline type");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline auto convert(Load_op op)
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

inline auto convert(Store_op op) noexcept
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

inline auto convert(Present_mode mode)
{
    switch (mode) {
        case Present_mode::immediate:
            return VK_PRESENT_MODE_IMMEDIATE_KHR;
        case Present_mode::mailbox:
            return VK_PRESENT_MODE_MAILBOX_KHR;
        case Present_mode::fifo:
            return VK_PRESENT_MODE_FIFO_KHR;
        case Present_mode::fifo_relaxed:
            return VK_PRESENT_MODE_FIFO_RELAXED_KHR;
        default:
            throw std::runtime_error("invalid the present mode");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline auto convert(Color_space space)
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
