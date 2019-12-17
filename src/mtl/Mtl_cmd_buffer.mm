//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include <cstdint>
#include <sc/Msl_compiler.h>
#include "std_lib_modules.h"
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

constexpr auto max_arg_num = 16;

//----------------------------------------------------------------------------------------------------------------------

template<typename T>
inline bool operator!=(const Mtl_arg<T>& lhs, const Mtl_arg<T>& rhs)
{
    return lhs.res != rhs.res;
}

//----------------------------------------------------------------------------------------------------------------------

template<>
inline bool operator!=(const Mtl_arg<Mtl_buffer>& lhs, const Mtl_arg<Mtl_buffer>& rhs)
{
    return lhs.res != rhs.res || lhs.offset != rhs.offset;
}

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

inline auto operator&(const Pipeline_stage& stage, const Pipeline_stages& stages)
{
    return stages.to_ulong() & etoi(stage);
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Mtl_arg_table::Mtl_arg_table() :
    buffers_ { max_arg_num },
    images_ { max_arg_num },
    samplers_ { max_arg_num }
{
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_arg_table::clear()
{
    buffers_.empty();
    images_.empty();
    samplers_.empty();
}

//----------------------------------------------------------------------------------------------------------------------

bool Mtl_arg_table::set(Mtl_arg<Mtl_buffer>&& arg, uint32_t index)
{
    auto changed = (arg != buffers_[index]);

    if (changed)
        buffers_[index] = arg;

    return changed;
}

//----------------------------------------------------------------------------------------------------------------------

bool Mtl_arg_table::set(Mtl_arg<Mtl_image>&& arg, uint32_t index)
{
    auto changed = (arg != images_[index]);

    if (changed)
        images_[index] = arg;

    return changed;
}

//----------------------------------------------------------------------------------------------------------------------

bool Mtl_arg_table::set(Mtl_arg<Mtl_sampler>&& arg, uint32_t index)
{
    auto changed = (arg != samplers_[index]);

    if (changed)
        samplers_[index] = arg;

    return changed;
}

//----------------------------------------------------------------------------------------------------------------------

Mtl_cmd_buffer::Mtl_cmd_buffer(Mtl_device* device) :
    device_ { device },
    command_buffer_ { nil },
    render_encoder_ { nil },
    compute_encoder_ { nil },
    vertex_buffers_ { nullptr, nullptr },
    index_buffer_ { nullptr },
    index_type_ { MTLIndexTypeUInt16 },
    arg_tables_ {},
    pipeline_ { nullptr }
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
    vertex_buffers_.fill(nullptr);
    index_buffer_ = nullptr;
    index_type_ = MTLIndexTypeUInt16;
    arg_tables_.clear();
    pipeline_ = nullptr;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::reset()
{
    command_buffer_ = nil;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::bind(Buffer* buffer, uint32_t index)
{
    auto mtl_buffer = static_cast<Mtl_buffer*>(buffer);

    if (mtl_buffer == vertex_buffers_[index])
        return;

    if (render_encoder_ && mtl_buffer)
        bind_buffer_(mtl_buffer, index);

    vertex_buffers_[index] = mtl_buffer;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::bind(Buffer* buffer, Index_type type)
{
    index_buffer_ = static_cast<Mtl_buffer*>(buffer);
    index_type_ = convert<MTLIndexType>(type);
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::bind(Buffer* buffer, const Pipeline_stages& stages, uint32_t index)
{
    auto mtl_buffer = static_cast<Mtl_buffer*>(buffer);

    if (Pipeline_stage::vertex & stages) {
        auto& arg_table = arg_tables_[Pipeline_stage::vertex];

        if (render_encoder_ && arg_table.set({ mtl_buffer, 0 }, index))
            bind_buffer_(mtl_buffer, Pipeline_stage::vertex, index);
    }

    if (Pipeline_stage::fragment & stages) {
        auto& arg_table = arg_tables_[Pipeline_stage::fragment];

        if (render_encoder_ && arg_table.set({ mtl_buffer, 0 }, index))
            bind_buffer_(mtl_buffer, Pipeline_stage::fragment, index);
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::bind(Image* image, const Pipeline_stages& stages, uint32_t index)
{
    auto mtl_image = static_cast<Mtl_image*>(image);

    if (Pipeline_stage::vertex & stages) {
        auto& arg_table = arg_tables_[Pipeline_stage::vertex];

        if (render_encoder_ && arg_table.set({ mtl_image }, index))
            bind_image_(mtl_image, Pipeline_stage::vertex, index);
    }

    if (Pipeline_stage::fragment & stages) {
        auto& arg_table = arg_tables_[Pipeline_stage::fragment];

        if (render_encoder_ && arg_table.set({ mtl_image }, index))
            bind_image_(mtl_image, Pipeline_stage::fragment, index);
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::bind(Sampler* sampler, const Pipeline_stages& stages, uint32_t index)
{
    auto mtl_sampler = static_cast<Mtl_sampler*>(sampler);

    if (Pipeline_stage::vertex & stages) {
        [render_encoder_ setVertexSamplerState:mtl_sampler->sampler_state() atIndex:index];
    }

    if (Pipeline_stage::fragment & stages) {
        [render_encoder_ setFragmentSamplerState:mtl_sampler->sampler_state() atIndex:index];
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::bind(Pipeline* pipeline)
{
    auto mtl_pipeline = static_cast<Mtl_pipeline*>(pipeline);

    if (mtl_pipeline == pipeline_)
        return;

    if (render_encoder_ && mtl_pipeline) {
        if (Pipeline_type::render == mtl_pipeline->type())
            bind_render_pipeline_(mtl_pipeline);
        else {
            bind_compute_pipeline_(mtl_pipeline);
        }
    }

    pipeline_ = mtl_pipeline;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::begin(const Render_pass_state& state)
{
    // configure a render pass descriptor.
    auto descriptor = [MTLRenderPassDescriptor renderPassDescriptor];

    for( auto i = 0; i != state.colors.size(); ++i) {
        auto& color = state.colors[i];

        if (is_valid(color)) {
            auto mtl_image = static_cast<Mtl_image*>(color.image);

            // set up the color attachment descriptor at the index.
            descriptor.colorAttachments[i].texture = mtl_image->texture();
            descriptor.colorAttachments[i].loadAction = convert<MTLLoadAction>(color.load_op);
            descriptor.colorAttachments[i].storeAction = convert<MTLStoreAction>(color.store_op);
            descriptor.colorAttachments[i].clearColor = convert<MTLClearColor>(color.clear_value);
        }
    }

    auto& depth_stencil = state.depth_stencil;

    if (is_valid(depth_stencil)) {
        auto mtl_image = static_cast<Mtl_image*>(depth_stencil.image);

        // set up the depth attachment descriptor.
        descriptor.depthAttachment.texture = mtl_image->texture();
        descriptor.depthAttachment.loadAction = convert<MTLLoadAction>(depth_stencil.load_op);
        descriptor.depthAttachment.storeAction = convert<MTLStoreAction>(depth_stencil.store_op);
        descriptor.depthAttachment.clearDepth = depth_stencil.clear_value.d;

        // set up the stencil attachment descriptor.
        descriptor.stencilAttachment.texture = mtl_image->texture();
        descriptor.stencilAttachment.loadAction = descriptor.depthAttachment.loadAction;
        descriptor.stencilAttachment.storeAction = descriptor.depthAttachment.storeAction;
        descriptor.stencilAttachment.clearStencil = depth_stencil.clear_value.s;
    }

    // start render encoding.
    render_encoder_ = [command_buffer_ renderCommandEncoderWithDescriptor:descriptor];

    // by default, the front facing is the clockwise in metal.
    // change the front facing winding to counter clockwise.
    [render_encoder_ setFrontFacingWinding:MTLWindingCounterClockwise];

    // bind vertex buffers.
    for (auto i = 0; i != vertex_buffers_.size(); ++i) {
        auto& binding_vertex_buffer = vertex_buffers_[i];

        if (!binding_vertex_buffer)
            continue;

        bind_buffer_(binding_vertex_buffer, i);
    }

    // bind argument buffers.
    for (auto& [stage, arg_table] : arg_tables_) {
        for (auto i = 0; i != max_arg_num; ++i) {
            auto arg = arg_table.get<Mtl_buffer>(i);

            if (!arg.res)
                continue;

            bind_buffer_(arg.res, stage, i, arg.offset);
        }
    }

    // bind argument images.
    for (auto& [stage, arg_table] : arg_tables_) {
        for (auto i = 0; i != max_arg_num; ++i) {
            auto arg = arg_table.get<Mtl_image>(i);

            if (!arg.res)
                continue;

            bind_image_(arg.res, stage, i);
        }
    }

    // bind samplers.
    for (auto& [stage, arg_table] : arg_tables_) {
        for (auto i = 0; i != max_arg_num; ++i) {
            auto arg = arg_table.get<Mtl_sampler>(i);

            if (!arg.res)
                continue;

            bind_sampler_(arg.res, stage, i);
        }
    }

    // bind pipeline.
    if (pipeline_) {
        if (Pipeline_type::render == pipeline_->type())
            bind_render_pipeline_(pipeline_);
        else
            bind_compute_pipeline_(pipeline_);
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

    [render_encoder_ setViewport:convert<MTLViewport>(viewport)];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::set(const Scissor& scissor)
{
    assert(render_encoder_);

    [render_encoder_ setScissorRect:convert<MTLScissorRect>(scissor)];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::draw(uint32_t count, uint32_t first)
{
    assert(render_encoder_);

    [render_encoder_ drawPrimitives:pipeline_->primitive_type()
                        vertexStart:first
                        vertexCount:count];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::draw_indexed(uint32_t count, uint32_t first)
{
    assert(render_encoder_);

    [render_encoder_ drawIndexedPrimitives:pipeline_->primitive_type()
                                indexCount:count
                                 indexType:index_type_
                               indexBuffer:index_buffer_->buffer()
                         indexBufferOffset:first * byte_size(index_type_)];
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
                      sourceSize:convert<MTLSize>(region.image_extent)
                       toTexture:mtl_dst_image->texture()
                destinationSlice:region.image_subresource.array_layer
                destinationLevel:region.image_subresource.mip_level
               destinationOrigin:convert<MTLOrigin>(region.image_offset)];
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
                     sourceOrigin:convert<MTLOrigin>(region.image_offset)
                       sourceSize:convert<MTLSize>(region.image_extent)
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

void Mtl_cmd_buffer::bind_image_(Mtl_image* image, Pipeline_stage stage, uint32_t index)
{
    assert(render_encoder_);

    switch (stage) {
        case Pipeline_stage::vertex:
            [render_encoder_ setVertexTexture:image->texture() atIndex:index];
            break;
        case Pipeline_stage::fragment:
            [render_encoder_ setFragmentTexture:image->texture() atIndex:index];

            break;
        case Pipeline_stage::compute:
            [compute_encoder_ setTexture:image->texture() atIndex:index];
            break;
        default:
            break;
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_cmd_buffer::bind_sampler_(Mtl_sampler* sampler, Pipeline_stage stage, uint32_t index)
{
    assert(render_encoder_);

    switch (stage) {
        case Pipeline_stage::vertex:
            [render_encoder_ setVertexSamplerState:sampler->sampler_state() atIndex:index];
            break;
        case Pipeline_stage::fragment:
            [render_encoder_ setFragmentSamplerState:sampler->sampler_state() atIndex:index];
            break;
        case Pipeline_stage::compute:
            [compute_encoder_ setSamplerState:sampler->sampler_state() atIndex:index];
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
