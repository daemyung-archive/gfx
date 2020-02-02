//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include <sc/Msl_compiler.h>
#include "std_lib.h"
#include "mtl_lib.h"
#include "Mtl_cmd_buffer.h"
#include "Mtl_device.h"
#include "Mtl_buffer.h"
#include "Mtl_image.h"
#include "Mtl_sampler.h"
#include "Mtl_pipeline.h"

using namespace std;
using namespace Sc_lib;
using namespace Gfx_lib;

namespace {

//----------------------------------------------------------------------------------------------------------------------

inline auto& operator|=(uint32_t& stages, Pipeline_stage stage)
{
    return stages |= etoi(stage);
}

//----------------------------------------------------------------------------------------------------------------------

inline auto is_stage_set(uint64_t stages, Pipeline_stage stage)
{
    return stages & etoi(stage);
}

//----------------------------------------------------------------------------------------------------------------------

inline auto byte_size(Index_type type)
{
    switch (type) {
        case Index_type::uint16:
            return sizeof(uint16_t);
        case Index_type::uint32:
            return sizeof(uint32_t);
        default:
            throw runtime_error("invalid the index type");
    }
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Mtl_render_encoder::Mtl_render_encoder(const Render_encoder_desc& desc, Mtl_cmd_buffer* cmd_buffer) :
    Render_encoder(),
    cmd_buffer_ {cmd_buffer},
    render_command_encoder_ {nil},
    vertex_streams_ {},
    index_stream_ {},
    arg_table_ {},
    pipeline_ {nullptr}
{
    init_render_command_encoder_(desc);
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_render_encoder::end()
{
    [render_command_encoder_ endEncoding];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_render_encoder::draw(uint32_t count, uint32_t first)
{
    bind_arg_table_();

    auto input_assembly = pipeline_->input_assembly();

    [render_command_encoder_ drawPrimitives:to_MTLPrimitiveType(input_assembly.topology)
                                vertexStart:first
                                vertexCount:count];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_render_encoder::draw_indexed(uint32_t count, uint32_t first)
{
    bind_arg_table_();

    auto input_assembly = pipeline_->input_assembly();
    auto offset = first * byte_size(index_stream_.index_type);

    [render_command_encoder_ drawIndexedPrimitives:to_MTLPrimitiveType(input_assembly.topology)
                                        indexCount:count
                                         indexType:to_MTLIndexType(index_stream_.index_type)
                                       indexBuffer:index_stream_.buffer->buffer()
                                 indexBufferOffset:offset + index_stream_.offset];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_render_encoder::vertex_buffer(Buffer* buffer, uint64_t offset, uint32_t index)
{
    Mtl_vertex_stream vertex_stream {static_cast<Mtl_buffer*>(buffer), offset};

    if (vertex_stream == vertex_streams_[index])
        return;

    // set a vertex buffer.
    [render_command_encoder_ setVertexBuffer:vertex_stream.buffer->buffer() offset:vertex_stream.offset
                                     atIndex:index + vertex_buffer_index_offset];

    // update a vertex buffer at index.
    vertex_streams_[index] = vertex_stream;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_render_encoder::index_buffer(Buffer* buffer, uint64_t offset, Index_type index_type)
{
    Mtl_index_stream index_stream {static_cast<Mtl_buffer*>(buffer), offset, index_type};

    if (index_stream == index_stream_)
        return;

    // update an index buffer and an index type.
    index_stream_ = index_stream;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_render_encoder::shader_buffer(Buffer* buffer, uint32_t offset, uint32_t index)
{
    auto& arg_buffer = arg_table_.buffers[index];
    auto buffer_impl = static_cast<Mtl_buffer*>(buffer);

    if (arg_buffer.buffer != buffer_impl) {
        arg_buffer.buffer = buffer_impl;
        arg_buffer.buffer_stages = etoi(Pipeline_stage::invalid);
    }

    if (arg_buffer.offset != offset) {
        arg_buffer.offset = offset;
        arg_buffer.offset_stages = etoi(Pipeline_stage::invalid);
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_render_encoder::shader_texture(Image* image, Sampler* sampler, uint32_t index)
{
    auto& arg_texture = arg_table_.textures[index];
    auto image_impl = static_cast<Mtl_image*>(image);
    auto sampler_impl = static_cast<Mtl_sampler*>(sampler);

    if (arg_texture.image != image_impl) {
        arg_texture.image = image_impl;
        arg_texture.image_stages = etoi(Pipeline_stage::invalid);
    }

    if (arg_texture.sampler != sampler_impl) {
        arg_texture.sampler = sampler_impl;
        arg_texture.sampler_stages = etoi(Pipeline_stage::invalid);
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_render_encoder::pipeline(Pipeline* pipeline)
{
    pipeline_ = static_cast<Mtl_pipeline*>(pipeline);

    [render_command_encoder_ setRenderPipelineState:pipeline_->render_pipeline_state()];

    auto rasterization = pipeline_->rasterization();

    [render_command_encoder_ setCullMode:to_MTLCullMode(rasterization.cull_mode)];
    [render_command_encoder_ setFrontFacingWinding:to_MTLWinding(rasterization.front_face)];

    auto depth_stencil = pipeline_->depth_stencil();

    if (depth_stencil.depth_test)
        [render_command_encoder_ setDepthStencilState:pipeline_->depth_stencil_state()];

    if (depth_stencil.stencil_test) {
        [render_command_encoder_ setStencilFrontReferenceValue:depth_stencil.front_stencil.referece
                                            backReferenceValue:depth_stencil.back_stencil.referece];
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_render_encoder::viewport(const Viewport& viewport)
{
    [render_command_encoder_ setViewport:to_MTLViewport(viewport)];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_render_encoder::scissor(const Scissor& scissor)
{
    [render_command_encoder_ setScissorRect:to_MTLScissorRect(scissor)];
}

//----------------------------------------------------------------------------------------------------------------------

Cmd_buffer* Mtl_render_encoder::cmd_buffer() const
{
    return cmd_buffer_;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_render_encoder::init_render_command_encoder_(const Render_encoder_desc& desc)
{
    // configure a render pass descriptor.
    auto descriptor = [MTLRenderPassDescriptor renderPassDescriptor];

    for( auto i = 0; i != desc.colors.size(); ++i) {
        auto& color = desc.colors[i];

        if (color.image) {
            auto mtl_image = static_cast<Mtl_image*>(color.image);

            // set up the color attachment descriptor at the index.
            descriptor.colorAttachments[i].texture = mtl_image->texture();
            descriptor.colorAttachments[i].loadAction = to_MTLLoadAction(color.load_op);
            descriptor.colorAttachments[i].storeAction = to_MTLStoreAction(color.store_op);
            descriptor.colorAttachments[i].clearColor = to_MTLClearColor(color.clear_value);
        }
    }

    auto& depth_stencil = desc.depth_stencil;

    if (depth_stencil.image) {
        auto mtl_image = static_cast<Mtl_image*>(depth_stencil.image);

        // set up the depth attachment descriptor.
        descriptor.depthAttachment.texture = mtl_image->texture();
        descriptor.depthAttachment.loadAction = to_MTLLoadAction(depth_stencil.load_op);
        descriptor.depthAttachment.storeAction = to_MTLStoreAction(depth_stencil.store_op);
        descriptor.depthAttachment.clearDepth = depth_stencil.clear_value.d;

        // set up the stencil attachment descriptor.
        descriptor.stencilAttachment.texture = mtl_image->texture();
        descriptor.stencilAttachment.loadAction = descriptor.depthAttachment.loadAction;
        descriptor.stencilAttachment.storeAction = descriptor.depthAttachment.storeAction;
        descriptor.stencilAttachment.clearStencil = depth_stencil.clear_value.s;
    }

    // start render encoding.
    render_command_encoder_ = [cmd_buffer_->command_buffer() renderCommandEncoderWithDescriptor:descriptor];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_render_encoder::bind_arg_table_()
{
    auto reflection = pipeline_->reflection();

    for (auto& [index, stages] : reflection.buffers)
        bind_arg_buffer_(index, stages);

    for (auto& [index, stages] : reflection.textures)
        bind_arg_texture_(index, stages);
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_render_encoder::bind_arg_buffer_(uint64_t index, uint32_t stages)
{
    auto& arg_buffer = arg_table_.buffers[index];

    if (is_stage_set(stages, Pipeline_stage::vertex_shader)) {
        if (!is_stage_set(arg_buffer.buffer_stages, Pipeline_stage::vertex_shader)) {
            [render_command_encoder_ setVertexBuffer:arg_buffer.buffer->buffer()
                                              offset:arg_buffer.offset
                                             atIndex:index];
            arg_buffer.buffer_stages |= Pipeline_stage::vertex_shader;
            arg_buffer.offset_stages |= Pipeline_stage::vertex_shader;
        }
        else if (!is_stage_set(arg_buffer.offset_stages, Pipeline_stage::vertex_shader)) {
            [render_command_encoder_ setVertexBufferOffset:arg_buffer.offset
                                                   atIndex:index];
            arg_buffer.offset_stages |= Pipeline_stage::vertex_shader;
        }
    }

    if (is_stage_set(stages, Pipeline_stage::fragment_shader)) {
        if (!is_stage_set(arg_buffer.buffer_stages, Pipeline_stage::fragment_shader)) {
            [render_command_encoder_ setFragmentBuffer:arg_buffer.buffer->buffer()
                                                offset:arg_buffer.offset
                                               atIndex:index];
            arg_buffer.buffer_stages |= Pipeline_stage::fragment_shader;
            arg_buffer.offset_stages |= Pipeline_stage::fragment_shader;
        }
        else if (!is_stage_set(arg_buffer.offset_stages, Pipeline_stage::fragment_shader)) {
            [render_command_encoder_ setFragmentBufferOffset:arg_buffer.offset
                                                     atIndex:index];
            arg_buffer.offset_stages |= Pipeline_stage::fragment_shader;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_render_encoder::bind_arg_texture_(uint64_t index, uint32_t stages)
{
    auto& arg_texture = arg_table_.textures[index];

    if (is_stage_set(stages, Pipeline_stage::vertex_shader)) {
        if (!is_stage_set(arg_texture.image_stages, Pipeline_stage::vertex_shader)) {
            [render_command_encoder_ setVertexTexture:arg_texture.image->texture()
                                              atIndex:index];
            arg_texture.image_stages |= Pipeline_stage::vertex_shader;
        }

        if (!is_stage_set(arg_texture.sampler_stages, Pipeline_stage::vertex_shader)) {
            [render_command_encoder_ setVertexSamplerState:arg_texture.sampler->sampler_state()
                                                   atIndex:index];
            arg_texture.sampler_stages |= Pipeline_stage::vertex_shader;
        }
    }

    if (is_stage_set(stages, Pipeline_stage::fragment_shader)) {
        if (!is_stage_set(arg_texture.image_stages, Pipeline_stage::fragment_shader)) {
            [render_command_encoder_ setFragmentTexture:arg_texture.image->texture()
                                                atIndex:index];
            arg_texture.image_stages |= Pipeline_stage::fragment_shader;
        }

        if (!is_stage_set(arg_texture.sampler_stages, Pipeline_stage::fragment_shader)) {
            [render_command_encoder_ setFragmentSamplerState:arg_texture.sampler->sampler_state()
                                                     atIndex:index];
            arg_texture.sampler_stages |= Pipeline_stage::fragment_shader;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

Mtl_blit_encoder::Mtl_blit_encoder(const Blit_encoder_desc& desc, Mtl_cmd_buffer* cmd_buffer) :
    Blit_encoder(),
    cmd_buffer_ {cmd_buffer},
    blit_command_encoder_ {nil}
{
    init_blit_command_encoder_();
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_blit_encoder::copy(Buffer* src_buffer, Buffer* dst_buffer, const Buffer_copy_region& region)
{
    auto mtl_src_buffer = static_cast<Mtl_buffer*>(src_buffer);
    auto mtl_dst_buffer = static_cast<Mtl_buffer*>(dst_buffer);

    [blit_command_encoder_ copyFromBuffer:mtl_src_buffer->buffer()
                             sourceOffset:region.src_offset
                                 toBuffer:mtl_dst_buffer->buffer()
                        destinationOffset:region.dst_offset
                                     size:region.size];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_blit_encoder::copy(Buffer* src_buffer, Image* dst_image, const Buffer_image_copy_region& region)
{
    auto mtl_src_buffer = static_cast<Mtl_buffer*>(src_buffer);
    auto mtl_dst_image = static_cast<Mtl_image*>(dst_image);

    [blit_command_encoder_ copyFromBuffer:mtl_src_buffer->buffer()
                             sourceOffset:region.buffer_offset
                        sourceBytesPerRow:region.buffer_row_size
                      sourceBytesPerImage:region.buffer_row_size * region.buffer_image_height
                               sourceSize:to_MTLSize(region.image_extent)
                                toTexture:mtl_dst_image->texture()
                         destinationSlice:region.image_subresource.array_layer
                         destinationLevel:region.image_subresource.mip_level
                        destinationOrigin:to_MTLOrigin(region.image_offset)];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_blit_encoder::copy(Image* src_image, Buffer* dst_buffer, const Buffer_image_copy_region& region)
{
    auto mtl_src_image = static_cast<Mtl_image*>(src_image);
    auto mtl_dst_buffer = static_cast<Mtl_buffer*>(dst_buffer);

    [blit_command_encoder_ copyFromTexture:mtl_src_image->texture()
                               sourceSlice:region.image_subresource.array_layer
                               sourceLevel:region.image_subresource.mip_level
                              sourceOrigin:to_MTLOrigin(region.image_offset)
                                sourceSize:to_MTLSize(region.image_extent)
                                  toBuffer:mtl_dst_buffer->buffer()
                         destinationOffset:region.buffer_offset
                    destinationBytesPerRow:region.buffer_row_size
                  destinationBytesPerImage:region.buffer_row_size * region.buffer_image_height];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_blit_encoder::end()
{
    [blit_command_encoder_ endEncoding];
}

//----------------------------------------------------------------------------------------------------------------------

Cmd_buffer* Mtl_blit_encoder::cmd_buffer() const
{
    return cmd_buffer_;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_blit_encoder::init_blit_command_encoder_()
{
    blit_command_encoder_ = [cmd_buffer_->command_buffer() blitCommandEncoder];
}

//----------------------------------------------------------------------------------------------------------------------

Mtl_cmd_buffer::Mtl_cmd_buffer(Mtl_device* device) :
    device_ { device },
    command_buffer_ { nil }
{
    init_command_buffer_();
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Render_encoder> Mtl_cmd_buffer::create(const Render_encoder_desc& desc)
{
    return make_unique<Mtl_render_encoder>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Blit_encoder> Mtl_cmd_buffer::create(const Blit_encoder_desc& desc)
{
    return make_unique<Mtl_blit_encoder>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::end()
{
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::reset()
{
    init_command_buffer_();
}

//----------------------------------------------------------------------------------------------------------------------

Device* Mtl_cmd_buffer::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::init_command_buffer_()
{
    command_buffer_ = [device_->command_queue() commandBuffer];
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
