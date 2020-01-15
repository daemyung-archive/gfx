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
#include "Vlk_set_layout.h"

using namespace std;
using namespace Sc_lib;
using namespace Gfx_lib;

namespace {

//----------------------------------------------------------------------------------------------------------------------

inline void execute(function<void ()>& func)
{
    func();
}

//----------------------------------------------------------------------------------------------------------------------

inline Viewport to_viewport(const Extent& extent)
{
    return {0.0f, 0.0f, static_cast<float>(extent.w), static_cast<float>(extent.h)};
}

//----------------------------------------------------------------------------------------------------------------------

inline Scissor to_scissor(const Extent& extent)
{
    return {0, 0, extent.w, extent.h};
}

//----------------------------------------------------------------------------------------------------------------------

inline auto to_render_pass_desc(const Render_encoder_desc& desc)
{
    Vlk_render_pass_desc render_pass_desc {};

    for (auto i = 0; i != 4; ++i) {
        auto& color = desc.colors[i];

        if (!color.image)
            continue;

        render_pass_desc.colors[i].format = color.image->format();
        render_pass_desc.colors[i].samples = color.image->samples();
        render_pass_desc.colors[i].load_op = color.load_op;
        render_pass_desc.colors[i].store_op = color.store_op;
    }

    auto& depth_stencil = desc.depth_stencil;

    if (depth_stencil.image) {
        render_pass_desc.depth_stencil.format = depth_stencil.image->format();
        render_pass_desc.depth_stencil.samples = depth_stencil.image->samples();
        render_pass_desc.depth_stencil.load_op = depth_stencil.load_op;
        render_pass_desc.depth_stencil.store_op = depth_stencil.store_op;
    }

    return render_pass_desc;
}

//----------------------------------------------------------------------------------------------------------------------

inline auto to_framebuffer_desc(Vlk_render_pass* render_pass, const Render_encoder_desc& desc)
{
    Vlk_framebuffer_desc framebuffer_desc {};

    framebuffer_desc.render_pass = render_pass;

    for (auto i = 0; i != 4; ++i) {
        auto& color = desc.colors[i];

        if (!color.image)
            continue;

        framebuffer_desc.colors[i] = static_cast<Vlk_image*>(color.image);
    }

    auto& depth_stencil = desc.depth_stencil;

    if (depth_stencil.image)
        framebuffer_desc.depth_stencil = static_cast<Vlk_image*>(depth_stencil.image);

    return framebuffer_desc;
}

//----------------------------------------------------------------------------------------------------------------------

}

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Vlk_arg_table::Vlk_arg_table() :
    args_ {}
{
    clear();
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_arg_table::clear()
{
    for (auto& args : args_)
        args.fill({});
}

//----------------------------------------------------------------------------------------------------------------------

Vlk_render_encoder::Vlk_render_encoder(const Render_encoder_desc& desc,
                                       Vlk_device* device, Vlk_cmd_buffer* cmd_buffer) :
    Render_encoder(),
    device_ {device},
    cmd_buffer_ {cmd_buffer},
    cmds_ {},
    vertex_buffers_ {nullptr, nullptr},
    index_buffer_ {nullptr},
    arg_tables_ {},
    pipeline_ {nullptr},
    render_pass_ {nullptr},
    framebuffer_ {nullptr},
    viewport_ {0.0f, 0.0f, 0.0f, 0.0f},
    scissor_ {0, 0, 0, 0}
{
    begin_render_pass_(desc);
    viewport(to_viewport(framebuffer_->extent()));
    scissor(to_scissor(framebuffer_->extent()));
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_render_encoder::end()
{
    end_render_pass_();

    for (auto& [priority, cmds] : cmds_)
        for_each(cmds, execute);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_render_encoder::draw(uint32_t count, uint32_t first)
{
    update_desc_sets_();
    bind_desc_sets_();

    cmds_[2].push_back([=]() {
        vkCmdDraw(cmd_buffer_->command_buffer(), count, 1, first, 0);
    });
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_render_encoder::draw_indexed(uint32_t count, uint32_t first)
{
    update_desc_sets_();
    bind_desc_sets_();

    cmds_[2].push_back([=]() {
        vkCmdDrawIndexed(cmd_buffer_->command_buffer(), count, 1, first, 0, 0);
    });
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_render_encoder::vertex_buffer(Buffer* buffer, uint32_t index)
{
    auto buffer_impl = static_cast<Vlk_buffer*>(buffer);

    if (buffer_impl == vertex_buffers_[index])
        return;

    cmds_[2].push_back([=]() {
        VkDeviceSize offset { 0 };
        
        vkCmdBindVertexBuffers(cmd_buffer_->command_buffer(),
                               index, 1, &buffer_impl->buffer(), &offset);
    });

    vertex_buffers_[index] = buffer_impl;
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_render_encoder::index_buffer(Buffer* buffer, Index_type index_type)
{
    auto buffer_impl = static_cast<Vlk_buffer*>(buffer);

    if (buffer_impl == index_buffer_)
        return;

    cmds_[2].push_back([=]() {
        vkCmdBindIndexBuffer(cmd_buffer_->command_buffer(),
                             buffer_impl->buffer(), 0, to_VkIndexType(index_type));
    });

    // update an index buffer.
    index_buffer_ = buffer_impl;
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_render_encoder::shader_buffer(Pipeline_stage stage, Buffer* buffer, uint32_t offset, uint32_t index)
{
    auto buffer_impl = static_cast<Vlk_buffer*>(buffer);
    auto& args = arg_tables_[stage][0];

    if (buffer_impl != args[index].buffer) {
        args.dirty_flags = 0x1;
        args[index].buffer = buffer_impl;
    }

    if (offset != args[index].offset) {
        args[index].dirty_flags = 0x1;
        args[index].offset = offset;
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_render_encoder::shader_texture(Pipeline_stage stage, Image* image, Sampler* sampler, uint32_t index)
{
    auto image_impl = static_cast<Vlk_image*>(image);
    auto sampler_impl = static_cast<Vlk_sampler*>(sampler);

    cmds_[0].push_back([=]() {
        if (VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL == image_impl->layout())
            return;

        // configure an image barrier.
        VkImageMemoryBarrier barrier {};

        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.srcAccessMask = image_impl->access_mask();
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.oldLayout = image_impl->layout();
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image_impl->image();
        barrier.subresourceRange.aspectMask = image_impl->aspect_mask();
        barrier.subresourceRange.levelCount = image_impl->mip_levels();
        barrier.subresourceRange.layerCount = image_impl->array_layers();

        // update image meta data.
        image_impl->access_mask_ = VK_ACCESS_SHADER_READ_BIT;
        image_impl->layout_ = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        // record barrier command.
        vkCmdPipelineBarrier(cmd_buffer_->command_buffer(),
                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                             VK_DEPENDENCY_BY_REGION_BIT,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);
    });

    auto& args = arg_tables_[stage][1];

    if (image_impl != args[index].image) {
        args.dirty_flags |= 0x1;
        args[index].image = image_impl;
    }

    if (sampler_impl != args[index].sampler) {
        args.dirty_flags |= 0x2;
        args[index].sampler = sampler_impl;
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_render_encoder::pipeline(Pipeline* pipeline)
{
    auto pipeline_impl = static_cast<Vlk_pipeline*>(pipeline);

    if (pipeline_impl == pipeline_)
        return;

    cmds_[2].push_back([=]() {
        vkCmdBindPipeline(cmd_buffer_->command_buffer(),
                          VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_impl->pipeline());
    });

    // update a pipeline.
    pipeline_ = pipeline_impl;
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_render_encoder::viewport(const Viewport& viewport)
{
    if (viewport == viewport_)
        return;

    cmds_[2].push_back([=]() {
        auto vk_viewport = to_VkViewport(viewport);

        vkCmdSetViewport(cmd_buffer_->command_buffer(), 0, 1, &vk_viewport);
    });

    viewport_ = viewport;
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_render_encoder::scissor(const Scissor& scissor)
{
    if (scissor == scissor_)
        return;

    cmds_[2].push_back([=]() {
        auto vk_scissor = to_VkRect2D(scissor);

        vkCmdSetScissor(cmd_buffer_->command_buffer(), 0, 1, &vk_scissor);
    });

    scissor_ = scissor;
}

//----------------------------------------------------------------------------------------------------------------------

Cmd_buffer* Vlk_render_encoder::cmd_buffer() const
{
    return cmd_buffer_;
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_render_encoder::begin_render_pass_(const Render_encoder_desc& desc)
{
    render_pass_ = device_->render_pass(to_render_pass_desc(desc));
    framebuffer_ = device_->framebuffer(to_framebuffer_desc(render_pass_, desc));

    cmds_[0].push_back([=]() {
        vector<VkImageMemoryBarrier> barriers;

        for (auto& color : desc.colors) {
            if (!color.image)
                continue;

            auto image_impl = static_cast<Vlk_image*>(color.image);

            if (VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL == image_impl->layout())
                continue;

            // configure an image barrier.
            VkImageMemoryBarrier barrier {};

            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.oldLayout = image_impl->layout();
            barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = image_impl->image();
            barrier.subresourceRange.aspectMask = image_impl->aspect_mask();
            barrier.subresourceRange.levelCount = image_impl->mip_levels();
            barrier.subresourceRange.layerCount = image_impl->array_layers();

            barriers.push_back(barrier);

            // update image meta data.
            image_impl->access_mask_ = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            image_impl->layout_ = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }

        auto& depth_stencil = desc.depth_stencil;

        if (depth_stencil.image) {
            auto image_impl = static_cast<Vlk_image*>(depth_stencil.image);

            if (VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL != image_impl->layout()) {
                // configure an image barrier.
                VkImageMemoryBarrier barrier {};

                barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                barrier.oldLayout = image_impl->layout();
                barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.image = image_impl->image();
                barrier.subresourceRange.aspectMask = image_impl->aspect_mask();
                barrier.subresourceRange.levelCount = image_impl->mip_levels();
                barrier.subresourceRange.layerCount = image_impl->array_layers();

                barriers.push_back(barrier);

                // update image meta data.
                image_impl->access_mask_ = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                image_impl->layout_ = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            }
        }

        vkCmdPipelineBarrier(cmd_buffer_->command_buffer(),
                             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             VK_DEPENDENCY_BY_REGION_BIT,
                             0, nullptr,
                             0, nullptr,
                             barriers.size(), &barriers[0]);
    });

    cmds_[1].push_back([=]() {
        vector<VkClearValue> clear_values;

        for (auto& color : desc.colors) {
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

        auto& depth_stencil = desc.depth_stencil;

        if (depth_stencil.image) {
            if (Load_op::clear == depth_stencil.load_op) {
                // configure clear value.
                VkClearValue clear_value;

                clear_value.depthStencil.depth = depth_stencil.clear_value.d;
                clear_value.depthStencil.stencil = depth_stencil.clear_value.s;

                clear_values.push_back(clear_value);
            }
        }

        // configure a render pass begin info.
        VkRenderPassBeginInfo begin_info {};

        begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        begin_info.renderPass = render_pass_->render_pass();
        begin_info.framebuffer = framebuffer_->framebuffer();
        begin_info.renderArea.extent = to_VkExtent2D(framebuffer_->extent());
        begin_info.clearValueCount = clear_values.size();
        begin_info.pClearValues = &clear_values[0];

        vkCmdBeginRenderPass(cmd_buffer_->command_buffer(), &begin_info, VK_SUBPASS_CONTENTS_INLINE);
    });
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_render_encoder::end_render_pass_()
{
    cmds_[3].push_back([=]() {
        vkCmdEndRenderPass(cmd_buffer_->command_buffer());
    });
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_render_encoder::update_desc_sets_()
{
    for (auto& [stage, arg_table] : arg_tables_) {
        unordered_map<uint32_t, VkDescriptorBufferInfo> buffer_infos;

        if (arg_table[0].dirty_flags) {
            arg_table[0].dirty_flags = 0;

            auto set_layout = pipeline_->set_layout(stage, 0);

            arg_table[0].desc_set = set_layout->desc_set();

            for (auto i = 0; i != 16; ++i) {
                if (!arg_table[0][i].buffer)
                    continue;

                VkDescriptorBufferInfo buffer_info {};

                buffer_info.buffer = arg_table[0][i].buffer->buffer();
                buffer_info.offset = 0;
                buffer_info.range = VK_WHOLE_SIZE;

                buffer_infos.insert({i, buffer_info});
            }
        }

        if (!buffer_infos.empty()) {
            vector<VkWriteDescriptorSet> write_desc_sets;

            for (auto& [binding, buffer_info] : buffer_infos) {
                VkWriteDescriptorSet write_desc_set {};

                write_desc_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write_desc_set.dstSet = arg_table[0].desc_set;
                write_desc_set.dstBinding = binding;
                write_desc_set.descriptorCount = 1;
                write_desc_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
                write_desc_set.pBufferInfo = &buffer_info;

                write_desc_sets.push_back(write_desc_set);
            }

            vkUpdateDescriptorSets(device_->device(),
                                   write_desc_sets.size(), &write_desc_sets[0],
                                   0, nullptr);
        }

        unordered_map<uint32_t, VkDescriptorImageInfo> image_infos;

        if (arg_table[1].dirty_flags) {
            arg_table[1].dirty_flags = 0;

            auto set_layout = pipeline_->set_layout(stage, 1);

            arg_table[1].desc_set = set_layout->desc_set();

            for (auto i = 0; i != 16; ++i) {
                if (!arg_table[1][i].image && !arg_table[1][i].sampler)
                    continue;

                VkDescriptorImageInfo image_info {};

                image_info.sampler = arg_table[1][i].sampler->sampler();
                image_info.imageView = arg_table[1][i].image->image_view();
                image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                image_infos.insert({i, image_info});
            }
        }

        if (!image_infos.empty()) {
            vector<VkWriteDescriptorSet> write_desc_sets;

            for (auto& [binding, image_info] : image_infos) {
                VkWriteDescriptorSet write_desc_set {};

                write_desc_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write_desc_set.dstSet = arg_table[1].desc_set;
                write_desc_set.dstBinding = binding;
                write_desc_set.descriptorCount = 1;
                write_desc_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                write_desc_set.pImageInfo = &image_info;

                write_desc_sets.push_back(write_desc_set);
            }

            vkUpdateDescriptorSets(device_->device(),
                                   write_desc_sets.size(), &write_desc_sets[0],
                                   0, nullptr);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_render_encoder::bind_desc_sets_()
{
    for (auto& [stage, arg_table] : arg_tables_) {
        vector<VkDescriptorSet> desc_sets;
        vector<uint32_t> offsets;

        if (arg_table[0].desc_set) {
            desc_sets.push_back(arg_table[0].desc_set);

            for (auto i = 0; i != 16; ++i) {
                if (!arg_table[0][i].buffer)
                    continue;

                offsets.push_back(arg_table[0][i].offset);
            }
        }

        if (arg_table[1].desc_set) {
            desc_sets.push_back(arg_table[1].desc_set);
        }

        if (desc_sets.empty())
            continue;

        auto pipeline_layout = pipeline_->pipeline_layout();

        cmds_[2].push_back([=]() {
            vkCmdBindDescriptorSets(cmd_buffer_->command_buffer(),
                                    VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout,
                                    0,
                                    static_cast<uint32_t>(desc_sets.size()), &desc_sets[0],
                                    static_cast<uint32_t>(offsets.size()), &offsets[0]);
        });
    }
}

//----------------------------------------------------------------------------------------------------------------------

Vlk_blit_encoder::Vlk_blit_encoder(const Blit_encoder_desc& desc, Vlk_cmd_buffer* cmd_buffer) :
    Blit_encoder(),
    cmd_buffer_ {cmd_buffer},
    cmds_ {}
{
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_blit_encoder::copy(Buffer* src_buffer, Buffer* dst_buffer, const Buffer_copy_region& region)
{
    auto src_buffer_impl = static_cast<Vlk_buffer*>(src_buffer);
    auto dst_buffer_impl = static_cast<Vlk_buffer*>(dst_buffer);

    cmds_.push_back([=]() {
        // configure buffer copy.
        VkBufferCopy copy {};

        copy.srcOffset = region.src_offset;
        copy.dstOffset = region.dst_offset;
        copy.size = region.size;

        // record a copy command.
        vkCmdCopyBuffer(cmd_buffer_->command_buffer(),
                        src_buffer_impl->buffer(), dst_buffer_impl->buffer(), 1, &copy);
    });
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_blit_encoder::copy(Buffer* src_buffer, Image* dst_image, const Buffer_image_copy_region& region)
{
    auto src_buffer_impl = static_cast<Vlk_buffer*>(src_buffer);
    auto dst_image_impl = static_cast<Vlk_image*>(dst_image);

    cmds_.push_back([=]() {
        if (VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL == dst_image_impl->layout())
            return;

        // configure an image barrier.
        VkImageMemoryBarrier barrier {};

        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.oldLayout = dst_image_impl->layout();
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = dst_image_impl->image();
        barrier.subresourceRange.aspectMask = dst_image_impl->aspect_mask();
        barrier.subresourceRange.levelCount = dst_image_impl->mip_levels();
        barrier.subresourceRange.layerCount = dst_image_impl->array_layers();

        // update image meta data.
        dst_image_impl->access_mask_ = VK_ACCESS_TRANSFER_WRITE_BIT;
        dst_image_impl->layout_ = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

        // recoard a barrier command.
        vkCmdPipelineBarrier(cmd_buffer_->command_buffer(),
                             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_DEPENDENCY_BY_REGION_BIT,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);
    });

    cmds_.push_back([=]() {
        // configure a buffer image copy.
        VkBufferImageCopy copy {};

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
        vkCmdCopyBufferToImage(cmd_buffer_->command_buffer(),
                               src_buffer_impl->buffer(),
                               dst_image_impl->image(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               1, &copy);
    });
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_blit_encoder::copy(Image* src_image, Buffer* dst_buffer, const Buffer_image_copy_region& region)
{
    auto src_image_impl = static_cast<Vlk_image*>(src_image);
    auto dst_buffer_impl = static_cast<Vlk_buffer*>(dst_buffer);

    cmds_.push_back([=]() {
        if (VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL == src_image_impl->layout())
            return;

        // configure an image barrier.
        VkImageMemoryBarrier barrier {};

        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.srcAccessMask = src_image_impl->access_mask();
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.oldLayout = src_image_impl->layout();
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = src_image_impl->image();
        barrier.subresourceRange.aspectMask = src_image_impl->aspect_mask();
        barrier.subresourceRange.levelCount = src_image_impl->mip_levels();
        barrier.subresourceRange.layerCount = src_image_impl->array_layers();

        // update image meta data.
        src_image_impl->access_mask_ = VK_ACCESS_TRANSFER_READ_BIT;
        src_image_impl->layout_ = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

        // record barrier command.
        vkCmdPipelineBarrier(cmd_buffer_->command_buffer(),
                             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                             VK_DEPENDENCY_BY_REGION_BIT,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);
    });

    cmds_.push_back([=]() {
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
        vkCmdCopyImageToBuffer(cmd_buffer_->command_buffer(),
                               src_image_impl->image(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                               dst_buffer_impl->buffer(),
                               1, &copy);
    });
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_blit_encoder::end()
{
    for_each(cmds_, execute);
}

//----------------------------------------------------------------------------------------------------------------------

Cmd_buffer* Vlk_blit_encoder::cmd_buffer() const
{
    return cmd_buffer_;
}

//----------------------------------------------------------------------------------------------------------------------

Vlk_cmd_buffer::Vlk_cmd_buffer(Vlk_device* device) :
    device_ {device},
    command_buffer_ {VK_NULL_HANDLE}
{
    init_command_buffer_();
    begin_command_buffer_();
}

//----------------------------------------------------------------------------------------------------------------------

Vlk_cmd_buffer::~Vlk_cmd_buffer()
{
    fini_command_buffer_();
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Render_encoder> Vlk_cmd_buffer::create(const Render_encoder_desc& desc)
{
    return make_unique<Vlk_render_encoder>(desc, device_, this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Blit_encoder> Vlk_cmd_buffer::create(const Blit_encoder_desc& desc)
{
    return make_unique<Vlk_blit_encoder>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::end()
{
    vkEndCommandBuffer(command_buffer_);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_cmd_buffer::reset()
{
    vkResetCommandBuffer(command_buffer_, 0);
    begin_command_buffer_();
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

void Vlk_cmd_buffer::begin_command_buffer_()
{
    // configure the command buffer begin info.
    VkCommandBufferBeginInfo begin_info {};

    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    // start recording.
    vkBeginCommandBuffer(command_buffer_, &begin_info);
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
