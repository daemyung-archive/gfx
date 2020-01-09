
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

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Mtl_pipeline::Mtl_pipeline(const Pipeline_desc& desc, Mtl_device* device) :
    Pipeline(),
    device_ { device },
    primitive_type_ { to_MTLPrimitiveType(desc.input_assembly.topology) },
    cull_mode_ { to_MTLCullMode(desc.rasterization.cull_mode) },
    winding_ { to_MTLWinding(desc.rasterization.front_face) },
    depth_test_ { desc.depth_stencil.depth_test },
    stencil_test_ { desc.depth_stencil.stencil_test },
    front_stencil_reference_ { desc.depth_stencil.front_stencil.referece },
    back_stencil_reference_ { desc.depth_stencil.back_stencil.referece },
    render_pipeline_state_ { nil },
    depth_stencil_state_ { nil }
{
    init_render_pipeline_state_(desc);

    auto& depth_stencil_stage = desc.depth_stencil;

    if (depth_stencil_stage.depth_test)
        init_depth_stencil_state_(depth_stencil_stage);
}

//----------------------------------------------------------------------------------------------------------------------

Device* Mtl_pipeline::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_pipeline::init_render_pipeline_state_(const Pipeline_desc& desc)
{
    // configure a render pipeline descriptor.
    auto descriptor = [[MTLRenderPipelineDescriptor alloc] init];

    // configure vertex functions.
    {
        auto shader_impl = static_cast<Mtl_shader*>(desc.vertex_shader);

        assert(shader_impl);
        descriptor.vertexFunction = shader_impl->function();
    }

    // configure fragment functions.
    {
        auto shader_impl = static_cast<Mtl_shader*>(desc.fragment_shader);

        assert(shader_impl);
        descriptor.fragmentFunction = shader_impl->function();
    }

    // configure buffer layouts and fetch behavior.
    {
        auto& vertex_input = desc.vertex_input;

        for (auto i = 0; i != vertex_input.bindings.size(); ++i) {
            auto& binding = vertex_input.bindings[i];

            if (UINT32_MAX == binding.stride)
                continue;

            auto index = i + vertex_buffer_index_offset;

            descriptor.vertexDescriptor.layouts[index].stride = binding.stride;
            descriptor.vertexDescriptor.layouts[index].stepRate = 1;
            descriptor.vertexDescriptor.layouts[index].stepFunction = to_MTLVertexStepFunction(binding.step_rate);
        }

        for (auto i = 0; i != vertex_input.attributes.size(); ++i ){
            auto& attribute = vertex_input.attributes[i];

            if (UINT32_MAX == attribute.binding)
                continue;

            auto index = attribute.binding + vertex_buffer_index_offset;

            descriptor.vertexDescriptor.attributes[i].format = to_MTLVertexFormat(attribute.format);
            descriptor.vertexDescriptor.attributes[i].offset = attribute.offset;
            descriptor.vertexDescriptor.attributes[i].bufferIndex = index;
        }
    }

    // configure the multisample state.
    {
        auto& multisample = desc.multisample;

        descriptor.sampleCount = multisample.samples;
    }

    // configure rendering pipeline state.
    {
        auto& color_blend = desc.color_blend;
        auto& output_merger = desc.output_merger;

        for (auto i = 0; i != 4; ++i) {
            auto& color_format = output_merger.color_formats[i];
            auto& attachment = color_blend.attachments[i];

            if (Format::invalid == color_format)
                continue;

            descriptor.colorAttachments[i].pixelFormat = to_MTLPixelFormat(color_format);
            descriptor.colorAttachments[i].writeMask = attachment.write_mask;
            descriptor.colorAttachments[i].blendingEnabled = attachment.blend;
            descriptor.colorAttachments[i].sourceRGBBlendFactor = to_MTLBlendFactor(attachment.src_rgb_blend_factor);
            descriptor.colorAttachments[i].destinationRGBBlendFactor = to_MTLBlendFactor(attachment.dst_rgb_blend_factor);
            descriptor.colorAttachments[i].rgbBlendOperation = to_MTLBlendOperation(attachment.rgb_blend_op);
            descriptor.colorAttachments[i].sourceAlphaBlendFactor = to_MTLBlendFactor(attachment.src_a_blend_factor);
            descriptor.colorAttachments[i].destinationAlphaBlendFactor = to_MTLBlendFactor(attachment.dst_a_blend_factor);
            descriptor.colorAttachments[i].alphaBlendOperation = to_MTLBlendOperation(attachment.a_blend_op);
        }

        auto& depth_stencil_format = output_merger.depth_stencil_format;

        if (Format::invalid != depth_stencil_format) {
            descriptor.depthAttachmentPixelFormat = to_MTLPixelFormat(depth_stencil_format);
            descriptor.stencilAttachmentPixelFormat = to_MTLPixelFormat(depth_stencil_format);
        }
    }

    // configure rasterization and visibility state.
    {
        auto& input_assembly = desc.input_assembly;

        descriptor.inputPrimitiveTopology = to_MTLPrimitiveTopologyClass(input_assembly.topology);
    }

    // try to create a render pipeline state.
    render_pipeline_state_ = [device_->device() newRenderPipelineStateWithDescriptor:descriptor
                                                                               error:nil];

    if (!render_pipeline_state_)
        throw runtime_error("fail to create the pipeline");
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_pipeline::init_depth_stencil_state_(const Depth_stencil& stage)
{
    // configure the depth stencil descriptor.
    auto descriptor = [[MTLDepthStencilDescriptor alloc] init];

    if (stage.depth_test) {
        descriptor.depthCompareFunction = to_MTLCompareFunction(stage.depth_compare_op);
        descriptor.depthWriteEnabled = stage.write_mask;
    }

    if (stage.stencil_test) {
        auto& front_stencil = stage.front_stencil;

        descriptor.frontFaceStencil.stencilFailureOperation = to_MTLStencilOperation(front_stencil.stencil_fail_op);
        descriptor.frontFaceStencil.depthFailureOperation = to_MTLStencilOperation(front_stencil.depth_fail_op );
        descriptor.frontFaceStencil.depthStencilPassOperation = to_MTLStencilOperation(front_stencil.depth_stencil_pass_op);
        descriptor.frontFaceStencil.stencilCompareFunction = to_MTLCompareFunction(front_stencil.compare_op);
        descriptor.frontFaceStencil.readMask = front_stencil.read_mask;
        descriptor.frontFaceStencil.writeMask = front_stencil.write_mask;

        auto& back_stencil = stage.back_stencil;

        descriptor.backFaceStencil.stencilFailureOperation = to_MTLStencilOperation(back_stencil.stencil_fail_op);
        descriptor.backFaceStencil.depthFailureOperation = to_MTLStencilOperation(back_stencil.depth_fail_op );
        descriptor.backFaceStencil.depthStencilPassOperation = to_MTLStencilOperation(back_stencil.depth_stencil_pass_op);
        descriptor.backFaceStencil.stencilCompareFunction = to_MTLCompareFunction(back_stencil.compare_op);
        descriptor.backFaceStencil.readMask = back_stencil.read_mask;
        descriptor.backFaceStencil.writeMask = back_stencil.write_mask;
    }

    // try to create a depth stencil state.
    depth_stencil_state_ = [device_->device() newDepthStencilStateWithDescriptor:descriptor];

    if (!depth_stencil_state_)
        throw runtime_error("fail to create the pipeline");
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
