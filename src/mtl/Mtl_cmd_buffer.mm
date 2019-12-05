//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include <cstdint>
#include <sc/Msl_compiler.h>
#include "mtl_lib_modules.h"
#include "std_lib_modules.h"
#include "Mtl_cmd_buffer.h"
#include "Mtl_device.h"
#include "Mtl_buffer.h"
#include "Mtl_image.h"
#include "Mtl_pipeline.h"

using namespace std;
using namespace Sc_lib;
using namespace Gfx_lib;

namespace {

//----------------------------------------------------------------------------------------------------------------------

inline auto byte_size(MTLIndexType type)
{
    switch (type) {
        case MTLIndexTypeUInt16:
            return sizeof(uint16_t);
        case MTLIndexTypeUInt32:
            return sizeof(uint32_t);
        default:
            throw runtime_error("invalid the index type");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline auto is_valid(const Render_pass_attachment_state& attachment)
{
    return attachment.image;
}

//----------------------------------------------------------------------------------------------------------------------

inline auto convert(const Clear_value& clear_value)
{
    return MTLClearColorMake(clear_value.r, clear_value.g, clear_value.b, clear_value.a);
}

//----------------------------------------------------------------------------------------------------------------------

inline auto make(const Render_pass_state& state)
{
    auto descriptor = [MTLRenderPassDescriptor renderPassDescriptor];

    for( auto i = 0; i != state.colors.size(); ++i) {
        auto& color = state.colors[i];

        if (is_valid(color)) {
            auto mtl_image = static_cast<Mtl_image*>(color.image);

            // set up the color attachment descriptor at the index.
            descriptor.colorAttachments[i].texture = mtl_image->texture();
            descriptor.colorAttachments[i].loadAction = convert(color.load_op);
            descriptor.colorAttachments[i].storeAction = convert(color.store_op);
            descriptor.colorAttachments[i].clearColor = convert(color.clear_value);
        }
    }

    auto& depth_stencil = state.depth_stencil;

    if (is_valid(depth_stencil)) {
        auto mtl_image = static_cast<Mtl_image*>(depth_stencil.image);

        // set up the depth attachment descriptor.
        descriptor.depthAttachment.texture = mtl_image->texture();
        descriptor.depthAttachment.loadAction = convert(depth_stencil.load_op);
        descriptor.depthAttachment.storeAction = convert(depth_stencil.store_op);
        descriptor.depthAttachment.clearDepth = depth_stencil.clear_value.d;

        // set up the stencil attachment descriptor.
        descriptor.stencilAttachment.texture = mtl_image->texture();
        descriptor.stencilAttachment.loadAction = convert(depth_stencil.load_op);
        descriptor.stencilAttachment.storeAction = convert(depth_stencil.store_op);
        descriptor.stencilAttachment.clearStencil = depth_stencil.clear_value.s;
    }

    return descriptor;
}

//----------------------------------------------------------------------------------------------------------------------

inline auto operator&(const Pipeline_stage& stage, const Pipeline_stages& stages)
{
    return stages.to_ulong() & etoi(stage);
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Mtl_cmd_buffer::Mtl_cmd_buffer(Mtl_device* device) :
    device_ { device },
    command_buffer_ { nil },
    render_encoder_ { nil },
    compute_encoder_ { nil },
    binding_vertex_buffers_ { nullptr, nullptr },
    binding_index_buffer_ { nullptr },
    binding_index_type_ { MTLIndexTypeUInt16 },
    binding_uniform_buffers_ {},
    binding_pipeline_ { nullptr }
{
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::start()
{
    command_buffer_ = [device_->command_queue() commandBuffer];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::stop()
{
    binding_vertex_buffers_.fill(nullptr);
    binding_index_buffer_ = nullptr;
    binding_index_type_ = MTLIndexTypeUInt16;
    binding_uniform_buffers_.clear();
    binding_uniform_offsets_.clear();
    binding_pipeline_ = nullptr;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::reset()
{
    command_buffer_ = nil;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::bind(Buffer* vertex_buffer, uint32_t index)
{
    auto mtl_vertex_buffer = static_cast<Mtl_buffer*>(vertex_buffer);

    if (mtl_vertex_buffer == binding_vertex_buffers_[index])
        return;

    if (render_encoder_ && mtl_vertex_buffer)
        bind_buffer_(mtl_vertex_buffer, index);

    binding_vertex_buffers_[index] = mtl_vertex_buffer;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::bind(Buffer* index_buffer, Index_type type)
{
    binding_index_buffer_ = static_cast<Mtl_buffer*>(index_buffer);
    binding_index_type_ = convert(type);
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::bind(Buffer* uniform_buffer, const Pipeline_stages& stages, uint32_t index)
{
    auto mtl_uniform_buffer = static_cast<Mtl_buffer*>(uniform_buffer);

    if (Pipeline_stage::vertex & stages) {
        auto& buffers = binding_uniform_buffers_[Pipeline_stage::vertex];
        auto& offsets = binding_uniform_offsets_[Pipeline_stage::vertex];

        if (render_encoder_ && (mtl_uniform_buffer != buffers[index]))
            bind_buffer_(mtl_uniform_buffer, Pipeline_stage::vertex, index);

        buffers[index] = mtl_uniform_buffer;
        offsets[index] = 0;
    }

    if (Pipeline_stage::fragment & stages) {
        auto& buffers = binding_uniform_buffers_[Pipeline_stage::fragment];
        auto& offsets = binding_uniform_offsets_[Pipeline_stage::fragment];

        if (render_encoder_ && (mtl_uniform_buffer != buffers[index])) {
            bind_buffer_(mtl_uniform_buffer, Pipeline_stage::fragment, index);
        }

        buffers[index] = mtl_uniform_buffer;
        offsets[index] = 0;
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::bind(Pipeline* pipeline)
{
    auto mtl_pipeline = static_cast<Mtl_pipeline*>(pipeline);

    if (mtl_pipeline == binding_pipeline_)
        return;

    if (render_encoder_ && mtl_pipeline) {
        if (Pipeline_type::render == mtl_pipeline->type())
            bind_render_pipeline_(mtl_pipeline);
        else {
            bind_compute_pipeline_(mtl_pipeline);
        }
    }

    binding_pipeline_ = mtl_pipeline;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::begin(const Render_pass_state& state)
{
    auto descriptor = make(state);

    render_encoder_ = [command_buffer_ renderCommandEncoderWithDescriptor:descriptor];

    // by default, the front facing is the clockwise in metal.
    // change the front facing winding to counter clockwise.
    [render_encoder_ setFrontFacingWinding:MTLWindingCounterClockwise];

    // bind vertex buffers.
    for (auto i = 0; i != binding_vertex_buffers_.size(); ++i) {
        auto& binding_vertex_buffer = binding_vertex_buffers_[i];

        if (!binding_vertex_buffer)
            continue;

        bind_buffer_(binding_vertex_buffer, i);
    }

    // bind uniform buffers.
    for (auto& [stage, buffers] : binding_uniform_buffers_) {
        auto& offsets = binding_uniform_offsets_[stage];

        for (auto i = 0; i != 8; ++i) {
            if (!buffers[i])
                continue;

            bind_buffer_(buffers[i], stage, i, offsets[i]);
        }
    }

    // bind images and samplers.

    // bind pipeline.
    if (binding_pipeline_) {
        if (Pipeline_type::render == binding_pipeline_->type())
            bind_render_pipeline_(binding_pipeline_);
        else
            bind_compute_pipeline_(binding_pipeline_);
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::end()
{
    [render_encoder_ endEncoding];
    render_encoder_ = nil;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::set(const Viewport& viewport)
{
    assert(render_encoder_);

    [render_encoder_ setViewport:convert(viewport)];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::set(const Scissor& scissor)
{
    assert(render_encoder_);

    [render_encoder_ setScissorRect:convert(scissor)];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::draw(uint32_t count, uint32_t first)
{
    assert(render_encoder_);

    [render_encoder_ drawPrimitives:binding_pipeline_->primitive_type()
                        vertexStart:first
                        vertexCount:count];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::draw_indexed(uint32_t count, uint32_t first)
{
    assert(render_encoder_);

    [render_encoder_ drawIndexedPrimitives:binding_pipeline_->primitive_type()
                                indexCount:count
                                 indexType:binding_index_type_
                               indexBuffer:binding_index_buffer_->buffer()
                         indexBufferOffset:first * byte_size(binding_index_type_)];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::copy(Buffer* src_buffer, Buffer* dst_buffer, const Buffer_copy_region& region)
{
    auto mtl_src_buffer = static_cast<Mtl_buffer*>(src_buffer);
    auto mtl_dst_buffer = static_cast<Mtl_buffer*>(dst_buffer);

    assert(mtl_src_buffer && mtl_dst_buffer);
    auto blit_encoder = [command_buffer_ blitCommandEncoder];

    [blit_encoder copyFromBuffer:mtl_src_buffer->buffer()
                    sourceOffset:region.src_offset
                        toBuffer:mtl_dst_buffer->buffer()
               destinationOffset:region.dst_offset
                            size:region.size];
    [blit_encoder endEncoding];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::copy(Buffer* src_buffer, Image* dst_image, const Buffer_image_copy_region& region)
{
    auto mtl_src_buffer = static_cast<Mtl_buffer*>(src_buffer);
    auto mtl_dst_image = static_cast<Mtl_image*>(dst_image);

    assert(mtl_src_buffer && mtl_dst_image);
    auto blit_encoder = [command_buffer_ blitCommandEncoder];

    [blit_encoder copyFromBuffer:mtl_src_buffer->buffer()
                    sourceOffset:region.buffer_offset
               sourceBytesPerRow:region.buffer_row_size
             sourceBytesPerImage:region.buffer_row_size * region.buffer_image_height
                      sourceSize:convert(region.image_extent)
                       toTexture:mtl_dst_image->texture()
                destinationSlice:region.image_subresource.array_layer
                destinationLevel:region.image_subresource.mip_level
               destinationOrigin:convert(region.image_offset)];
    [blit_encoder endEncoding];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::copy(Image* src_image, Buffer* dst_buffer, const Buffer_image_copy_region& region)
{
    auto mtl_src_image = static_cast<Mtl_image*>(src_image);
    auto mtl_dst_buffer = static_cast<Mtl_buffer*>(dst_buffer);

    assert(mtl_src_image && mtl_dst_buffer);
    auto blit_encoder = [command_buffer_ blitCommandEncoder];

    [blit_encoder copyFromTexture:mtl_src_image->texture()
                      sourceSlice:region.image_subresource.array_layer
                      sourceLevel:region.image_subresource.mip_level
                     sourceOrigin:convert(region.image_offset)
                       sourceSize:convert(region.image_extent)
                         toBuffer:mtl_dst_buffer->buffer()
                destinationOffset:region.buffer_offset
           destinationBytesPerRow:region.buffer_row_size
         destinationBytesPerImage:region.buffer_row_size * region.buffer_image_height];
    [blit_encoder endEncoding];
}

//----------------------------------------------------------------------------------------------------------------------

Device* Mtl_cmd_buffer::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::bind_buffer_(Mtl_buffer* buffer, uint32_t index)
{
    assert(render_encoder_);

    [render_encoder_ setVertexBuffer:buffer->buffer() offset:0 atIndex:index + vertex_buffer_index_offset];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::bind_buffer_(Mtl_buffer* buffer, Pipeline_stage stage, uint32_t index, uint32_t offset)
{
    assert(render_encoder_);

    switch (stage) {
        case Pipeline_stage::vertex:
            [render_encoder_ setVertexBuffer:buffer->buffer() offset:offset
                                     atIndex:index];

            break;
        case Pipeline_stage::fragment:
            [render_encoder_ setFragmentBuffer:buffer->buffer() offset:offset
                                       atIndex:index];

            break;
        case Pipeline_stage::compute:
            [compute_encoder_ setBuffer:buffer->buffer() offset:offset
                                atIndex:index];

            break;
        default:
            break;
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::bind_render_pipeline_(Mtl_pipeline* pipeline)
{
    assert(render_encoder_);

    [render_encoder_ setCullMode:pipeline->cull_mode()];
    [render_encoder_ setRenderPipelineState:pipeline->render_pipeline_state()];

    if (pipeline->depth_test_enabled())
        [render_encoder_ setDepthStencilState:pipeline->depth_stencil_state()];

    if (pipeline->stencil_test_enabled())
        [render_encoder_ setStencilFrontReferenceValue:pipeline->front_stencil_reference()
                                    backReferenceValue:pipeline->back_stencil_reference()];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::bind_compute_pipeline_(Mtl_pipeline* pipeline)
{
    // [compute_encoder_ setComputePipelineState]
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
