//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "std_lib.h"
#include "vlk_lib.h"
#include "Vlk_cmd_buffer.h"
#include "Vlk_device.h"
#include "Vlk_buffer.h"
#include "Vlk_image.h"
#include "Vlk_sampler.h"
#include "Vlk_pipeline.h"
#include "Vlk_render_pass.h"
#include "Vlk_framebuffer.h"

using namespace std;
using namespace Sc_lib;
using namespace Gfx_lib;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Vlk_cmd_buffer::Vlk_cmd_buffer(Vlk_device* device) :
    device_ { device },
    command_buffer_ { VK_NULL_HANDLE },
    vertex_buffers_ { nullptr, nullptr },
    index_buffer_ { nullptr },
    pipeline_ { nullptr }
{
    init_command_buffer_();
}

//----------------------------------------------------------------------------------------------------------------------

Vlk_cmd_buffer::~Vlk_cmd_buffer()
{
    fini_command_buffer_();
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::start()
{
    // configure the command buffer begin info.
    VkCommandBufferBeginInfo begin_info {};

    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    // start recording.
    vkBeginCommandBuffer(command_buffer_, &begin_info);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::stop()
{
    // end recording.
    vkEndCommandBuffer(command_buffer_);

    // reset binding states.
    vertex_buffers_.fill(nullptr);
    index_buffer_ = nullptr;
    pipeline_ = nullptr;
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::reset()
{
    vkResetCommandBuffer(command_buffer_, 0);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::bind(Buffer* buffer, uint32_t index)
{
    // cast to the implementation.
    auto buffer_impl = static_cast<Vlk_buffer*>(buffer);

    if (buffer_impl == vertex_buffers_[index])
        return;

    if (buffer_impl) {
        constexpr VkDeviceSize offset { 0 };

        vkCmdBindVertexBuffers(command_buffer_, index, 1, &buffer_impl->buffer(), &offset);
    }

    vertex_buffers_[index] = buffer_impl;
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::bind(Buffer* buffer, Index_type type)
{
    // cast to the implementation.
    auto buffer_impl = static_cast<Vlk_buffer*>(buffer);

    if (buffer_impl == index_buffer_)
        return;

    if (buffer_impl)
        vkCmdBindIndexBuffer(command_buffer_, buffer_impl->buffer(), 0, convert(type));

    index_buffer_ = buffer_impl;
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::bind(Buffer* buffer, const Pipeline_stages& stages, uint32_t index)
{
    assert(false);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::bind(Image* image, const Pipeline_stages& stages, uint32_t index)
{
    assert(false);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::bind(Sampler* sampler, const Pipeline_stages& stages, uint32_t index)
{
    assert(false);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::bind(Pipeline* pipeline)
{
    auto pipeline_impl = static_cast<Vlk_pipeline*>(pipeline);

    if (pipeline_impl == pipeline_)
        return;

    if (pipeline_impl)
        vkCmdBindPipeline(command_buffer_, convert(pipeline_impl->type()), pipeline_impl->pipeline());

    pipeline_ = pipeline_impl;
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::begin(const Render_pass_state& state)
{
    // collect image memory barriers.
    vector<VkImageMemoryBarrier> image_barriers;

    {
        for (auto& color : state.colors) {
            if (!color.image)
                break;

            // cast to the implementation.
            auto image_impl = static_cast<Vlk_image*>(color.image);

            // configure an image barrier.
            VkImageMemoryBarrier barrier {};

            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.oldLayout = image_impl->layout();
            barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            barrier.image = image_impl->image();
            barrier.subresourceRange.aspectMask = image_impl->aspect_mask();
            barrier.subresourceRange.levelCount = image_impl->mip_levels();
            barrier.subresourceRange.layerCount = image_impl->array_layers();

            image_barriers.push_back(barrier);

            // update image meta data.
            image_impl->access_mask_ = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            image_impl->layout_ = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }

        auto& depth_stencil = state.depth_stencil;

        if (depth_stencil.image) {
            // cast to the implementation.
            auto image_impl = static_cast<Vlk_image*>(depth_stencil.image);

            // configure an image barrier.
            VkImageMemoryBarrier barrier {};

            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            barrier.oldLayout = image_impl->layout();
            barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            barrier.image = image_impl->image();
            barrier.subresourceRange.aspectMask = image_impl->aspect_mask();
            barrier.subresourceRange.levelCount = image_impl->mip_levels();
            barrier.subresourceRange.layerCount = image_impl->array_layers();

            image_barriers.push_back(barrier);

            // update image meta data.
            image_impl->access_mask_ = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            image_impl->layout_ = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }
    }

    // record barrier command.
    vkCmdPipelineBarrier(command_buffer_,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                         VK_DEPENDENCY_BY_REGION_BIT,
                         0, nullptr,
                         0, nullptr,
                         image_barriers.size(), &image_barriers[0]);

    // collect clear colors.
    std::vector<VkClearValue> clear_values;

    {
        for (auto& color : state.colors) {
            if (!color.image)
                break;

            if (Load_op::clear != color.load_op)
                continue;

            // configure clear value.
            VkClearValue clear_value;

            clear_value.color.float32[0] = color.clear_value.r;
            clear_value.color.float32[1] = color.clear_value.g;
            clear_value.color.float32[2] = color.clear_value.b;
            clear_value.color.float32[3] = color.clear_value.a;

            clear_values.push_back(clear_value);
        }

        auto& depth_stencil = state.depth_stencil;

        if (depth_stencil.image && (Load_op::clear == depth_stencil.load_op)) {
            // configure clear value.
            VkClearValue clear_value;

            clear_value.depthStencil.depth = depth_stencil.clear_value.d;
            clear_value.depthStencil.stencil = depth_stencil.clear_value.s;

            clear_values.push_back(clear_value);
        }
    }

    // get a render pass and a framebuffer.
    auto render_pass = device_->render_pass(state);
    auto framebuffer = device_->framebuffer(state);

    // configure a render pass begin info.
    VkRenderPassBeginInfo begin_info {};

    begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    begin_info.renderPass = render_pass->render_pass();
    begin_info.framebuffer = framebuffer->framebuffer();
    begin_info.renderArea.extent = convert<VkExtent2D>(framebuffer->extent());
    begin_info.clearValueCount = clear_values.size();
    begin_info.pClearValues = &clear_values[0];

    vkCmdBeginRenderPass(command_buffer_, &begin_info, VK_SUBPASS_CONTENTS_INLINE);

    // configure viewport to the size of a framebuffer.
    VkViewport vk_viewport {
        0.0f,
        0.0f,
        static_cast<float>(framebuffer->extent().w),
        static_cast<float>(framebuffer->extent().h),
        0.0f,
        1.0f
    };

    // record a viewport command.
    vkCmdSetViewport(command_buffer_, 0, 1, &vk_viewport);

    // configure scissor to the size of a framebuffer.
    VkRect2D vk_scissor {
        { 0, 0 },
        { framebuffer->extent().w, framebuffer->extent().h }
    };

    // record a scissor command.
    vkCmdSetScissor(command_buffer_, 0, 1, &vk_scissor);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::end()
{
    vkCmdEndRenderPass(command_buffer_);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::set(const Viewport& viewport)
{
    VkViewport vk_viewport { viewport.x, viewport.y, viewport.w, viewport.h, 0.0f, 1.0f };

    vkCmdSetViewport(command_buffer_, 0, 1, &vk_viewport);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::set(const Scissor& scissor)
{
    VkRect2D vk_scissor {
        { static_cast<int32_t>(scissor.x), static_cast<int32_t>(scissor.y) },
        { scissor.w, scissor.h }
    };

    vkCmdSetScissor(command_buffer_, 0, 1, &vk_scissor);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::draw(uint32_t count, uint32_t first)
{
    vkCmdDraw(command_buffer_, count, 1, first, 0);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::draw_indexed(uint32_t count, uint32_t first)
{
    vkCmdDrawIndexed(command_buffer_, count, 1, first, 0, 0);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::copy(Buffer* src_buffer, Buffer* dst_buffer, const Buffer_copy_region& region)
{
    // cast to the implementation.
    auto src_buffer_impl = static_cast<Vlk_buffer*>(src_buffer);
    auto dst_buffer_impl = static_cast<Vlk_buffer*>(dst_buffer);

    // configure buffer copy.
    VkBufferCopy copy {};

    copy.srcOffset = region.src_offset;
    copy.dstOffset = region.dst_offset;
    copy.size = region.size;

    // record a copy command.
    vkCmdCopyBuffer(command_buffer_, src_buffer_impl->buffer(), dst_buffer_impl->buffer(), 1, &copy);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::copy(Buffer* src_buffer, Image* dst_image, const Buffer_image_copy_region& region)
{
    // cast to the implementation.
    auto src_buffer_impl = static_cast<Vlk_buffer*>(src_buffer);
    auto dst_image_impl = static_cast<Vlk_image*>(dst_image);

    // configure an image barrier.
    VkImageMemoryBarrier image_barrier {};

    image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_barrier.srcAccessMask = dst_image_impl->access_mask();
    image_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    image_barrier.oldLayout = dst_image_impl->layout();
    image_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    image_barrier.image = dst_image_impl->image();
    image_barrier.subresourceRange.aspectMask = dst_image_impl->aspect_mask();
    image_barrier.subresourceRange.levelCount = dst_image_impl->mip_levels();
    image_barrier.subresourceRange.layerCount = dst_image_impl->array_layers();

    // update image meta data.
    dst_image_impl->access_mask_ = VK_ACCESS_TRANSFER_WRITE_BIT;
    dst_image_impl->layout_ = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

    // recoard a barrier command.
    vkCmdPipelineBarrier(command_buffer_,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         VK_DEPENDENCY_BY_REGION_BIT,
                         0, nullptr,
                         0, nullptr,
                         1, &image_barrier);

    // configure a buffer image copy.
    VkBufferImageCopy copy {};

    copy.bufferOffset = region.buffer_offset;
    copy.bufferRowLength = region.buffer_row_size;
    copy.bufferImageHeight = region.buffer_image_height;
    copy.imageSubresource.aspectMask = dst_image_impl->aspect_mask();
    copy.imageSubresource.mipLevel = region.image_subresource.mip_level;
    copy.imageSubresource.baseArrayLayer = region.image_subresource.array_layer;
    copy.imageSubresource.layerCount = 1;
    copy.imageOffset.x = region.image_offset.x;
    copy.imageOffset.y = region.image_offset.y;
    copy.imageOffset.z = region.image_offset.z;
    copy.imageExtent.width = region.image_extent.w;
    copy.imageExtent.height = region.image_extent.h;
    copy.imageExtent.depth = region.image_extent.d;

    // record a copy command.
    vkCmdCopyBufferToImage(command_buffer_,
                           src_buffer_impl->buffer(), dst_image_impl->image(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1, &copy);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::copy(Image* src_image, Buffer* dst_buffer, const Buffer_image_copy_region& region)
{
    // cast to the implementation.
    auto src_image_impl = static_cast<Vlk_image*>(src_image);
    auto dst_buffer_impl = static_cast<Vlk_buffer*>(dst_buffer);

    // configure an image barrier.
    VkImageMemoryBarrier image_barrier {};

    image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_barrier.srcAccessMask = src_image_impl->access_mask();
    image_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    image_barrier.oldLayout = src_image_impl->layout();
    image_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    image_barrier.image = src_image_impl->image();
    image_barrier.subresourceRange.aspectMask = src_image_impl->aspect_mask();
    image_barrier.subresourceRange.levelCount = src_image_impl->mip_levels();
    image_barrier.subresourceRange.layerCount = src_image_impl->array_layers();

    // update image meta data.
    src_image_impl->access_mask_ = VK_ACCESS_TRANSFER_READ_BIT;
    src_image_impl->layout_ = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

    // record barrier command.
    vkCmdPipelineBarrier(command_buffer_,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         VK_DEPENDENCY_BY_REGION_BIT,
                         0, nullptr,
                         0, nullptr,
                         1, &image_barrier);

    // configure a buffer image copy.
    VkBufferImageCopy copy {};

    copy.bufferOffset = region.buffer_offset;
    copy.bufferRowLength = region.buffer_row_size;
    copy.bufferImageHeight = region.buffer_image_height;
    copy.imageSubresource.aspectMask = src_image_impl->aspect_mask();
    copy.imageSubresource.mipLevel = region.image_subresource.mip_level;
    copy.imageSubresource.baseArrayLayer = region.image_subresource.array_layer;
    copy.imageSubresource.layerCount = 1;
    copy.imageOffset.x = region.image_offset.x;
    copy.imageOffset.y = region.image_offset.y;
    copy.imageOffset.z = region.image_offset.z;
    copy.imageExtent.width = region.image_extent.w;
    copy.imageExtent.height = region.image_extent.h;
    copy.imageExtent.depth = region.image_extent.d;

    // record a copy command.
    vkCmdCopyImageToBuffer(command_buffer_,
                           src_image_impl->image(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst_buffer_impl->buffer(),
                           1, &copy);
}

//----------------------------------------------------------------------------------------------------------------------

Device* Vlk_cmd_buffer::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::init_command_buffer_()
{
    // configure a command buffer allocate info.
    VkCommandBufferAllocateInfo allocateInfo {};

    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = device_->command_pool();
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = 1;

    // try to create a command buffer.
    if (vkAllocateCommandBuffers(device_->device(), &allocateInfo, &command_buffer_))
        throw runtime_error("fail to create a cmd buffer");
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::fini_command_buffer_()
{
    vkFreeCommandBuffers(device_->device(), device_->command_pool(), 1, &command_buffer_);
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
