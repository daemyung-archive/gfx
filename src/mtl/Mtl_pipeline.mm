
//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include <sc/Msl_compiler.h>
#include "std_lib.h"
#include "mtl_lib.h"
#include "Mtl_pipeline.h"
#include "Mtl_device.h"
#include "Mtl_shader.h"

using namespace std;
using namespace Sc_lib;
using namespace Gfx_lib;

namespace {

//----------------------------------------------------------------------------------------------------------------------

inline auto to_MTLStencilDescriptor(const Stencil& stencil)
{
    auto descriptor = [MTLStencilDescriptor new];

    descriptor.stencilFailureOperation = to_MTLStencilOperation(stencil.stencil_fail_op);
    descriptor.depthFailureOperation = to_MTLStencilOperation(stencil.depth_fail_op );
    descriptor.depthStencilPassOperation = to_MTLStencilOperation(stencil.depth_stencil_pass_op);
    descriptor.stencilCompareFunction = to_MTLCompareFunction(stencil.compare_op);
    descriptor.readMask = stencil.read_mask;
    descriptor.writeMask = stencil.write_mask;

    return descriptor;
}

//----------------------------------------------------------------------------------------------------------------------

}

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Mtl_pipeline::Mtl_pipeline(const Pipeline_desc& desc, Mtl_device* device) :
    Pipeline {desc},
    device_ {device},
    render_pipeline_state_ {nil},
    depth_stencil_state_ {nil}
{
    init_render_pipeline_state_(desc.vertex_shader, desc.fragment_shader);
    init_depth_stencil_state_();
}

//----------------------------------------------------------------------------------------------------------------------

Device* Mtl_pipeline::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_pipeline::init_render_pipeline_state_(Shader* vertex_shader, Shader* fragment_shader)
{
    // configure a render pipeline descriptor.
    auto descriptor = [[MTLRenderPipelineDescriptor alloc] init];

    descriptor.vertexFunction = static_cast<Mtl_shader*>(vertex_shader)->function();
    descriptor.fragmentFunction = static_cast<Mtl_shader*>(fragment_shader)->function();

    auto vertexDescriptor = [MTLVertexDescriptor new];

    for (auto i = 0; i != 2; ++i) {
        if (UINT32_MAX == vertex_input_.bindings[i].stride)
            continue;

        auto layout = [MTLVertexBufferLayoutDescriptor new];

        layout.stride = vertex_input_.bindings[i].stride;
        layout.stepRate = 1;
        layout.stepFunction = to_MTLVertexStepFunction(vertex_input_.bindings[i].step_rate);

        vertexDescriptor.layouts[i + vertex_buffer_index_offset] = layout;
    }

    for (auto i = 0; i != 16; ++i ) {
        if (UINT32_MAX == vertex_input_.attributes[i].binding)
            continue;

        auto attribute = [MTLVertexAttributeDescriptor new];

        attribute.format = to_MTLVertexFormat(vertex_input_.attributes[i].format);
        attribute.offset = vertex_input_.attributes[i].offset;
        attribute.bufferIndex = vertex_input_.attributes[i].binding + vertex_buffer_index_offset;

        vertexDescriptor.attributes[i] = attribute;
    }

    descriptor.vertexDescriptor = vertexDescriptor;
    descriptor.sampleCount = multisample_.samples;

    for (auto i = 0; i != 4; ++i) {
        if (Format::invalid == output_merger_.color_formats[i])
            continue;

        const auto& attachment = color_blend_.attachments[i];
        auto colorAttachment = [MTLRenderPipelineColorAttachmentDescriptor new];

        colorAttachment.pixelFormat = to_MTLPixelFormat(output_merger_.color_formats[i]);
        colorAttachment.writeMask = attachment.write_mask;
        colorAttachment.blendingEnabled = attachment.blend;
        colorAttachment.sourceRGBBlendFactor = to_MTLBlendFactor(attachment.src_rgb_blend_factor);
        colorAttachment.destinationRGBBlendFactor = to_MTLBlendFactor(attachment.dst_rgb_blend_factor);
        colorAttachment.rgbBlendOperation = to_MTLBlendOperation(attachment.rgb_blend_op);
        colorAttachment.sourceAlphaBlendFactor = to_MTLBlendFactor(attachment.src_a_blend_factor);
        colorAttachment.destinationAlphaBlendFactor = to_MTLBlendFactor(attachment.dst_a_blend_factor);
        colorAttachment.alphaBlendOperation = to_MTLBlendOperation(attachment.a_blend_op);

        descriptor.colorAttachments[i] = colorAttachment;
    }

    if (Format::invalid != output_merger_.depth_stencil_format) {
        descriptor.depthAttachmentPixelFormat = to_MTLPixelFormat(output_merger_.depth_stencil_format);
        descriptor.stencilAttachmentPixelFormat = to_MTLPixelFormat(output_merger_.depth_stencil_format);
    }

    descriptor.inputPrimitiveTopology = to_MTLPrimitiveTopologyClass(input_assembly_.topology);

    // try to create a render pipeline state.
    render_pipeline_state_ = [device_->device() newRenderPipelineStateWithDescriptor:descriptor
                                                                               error:nil];

    if (!render_pipeline_state_)
        throw runtime_error("fail to create the pipeline");
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_pipeline::init_depth_stencil_state_()
{
    if (!depth_stencil_.depth_test && !depth_stencil_.stencil_test)
        return;

    // configure the depth stencil descriptor.
    auto descriptor = [[MTLDepthStencilDescriptor alloc] init];

    if (depth_stencil_.depth_test) {
        descriptor.depthCompareFunction = to_MTLCompareFunction(depth_stencil_.depth_compare_op);
        descriptor.depthWriteEnabled = depth_stencil_.write_mask;
    }

    if (depth_stencil_.stencil_test) {
        descriptor.frontFaceStencil = to_MTLStencilDescriptor(depth_stencil_.front_stencil);
        descriptor.backFaceStencil = to_MTLStencilDescriptor(depth_stencil_.back_stencil);
    }

    // try to create a depth stencil state.
    depth_stencil_state_ = [device_->device() newDepthStencilStateWithDescriptor:descriptor];

    if (!depth_stencil_state_)
        throw runtime_error("fail to create the pipeline");
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
