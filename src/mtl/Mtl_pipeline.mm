
//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include <sc/Msl_compiler.h>
#include "mtl_lib_modules.h"
#include "std_lib_modules.h"
#include "Mtl_pipeline.h"
#include "Mtl_device.h"
#include "Mtl_shader.h"

using namespace std;
using namespace Sc_lib;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Mtl_pipeline::Mtl_pipeline(const Pipeline_desc<Pipeline_type::render>& desc, Mtl_device* device) :
    Pipeline(),
    device_ { device },
    type_ { Pipeline_type::render },
    primitive_type_ { convert<MTLPrimitiveType>(desc.input_assembly_stage.topology) },
    cull_mode_ { convert(desc.rasterization_stage.cull_mode) },
    depth_test_enabled_ { desc.depth_stencil_stage.enable_depth_test },
    stencil_test_enabled_ { desc.depth_stencil_stage.enable_stencil_test },
    front_stencil_reference_ { desc.depth_stencil_stage.front_stencil_state.referece },
    back_stencil_reference_ { desc.depth_stencil_stage.back_stencil_state.referece },
    render_pipeline_state_ { nil },
    depth_stencil_state_ { nil }
{
    init_render_pipeline_state_(desc);

    auto& depth_stencil_stage = desc.depth_stencil_stage;

    if (depth_stencil_stage.enable_depth_test)
        init_depth_stencil_state_(depth_stencil_stage);
}

//----------------------------------------------------------------------------------------------------------------------

Device* Mtl_pipeline::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

Pipeline_type Mtl_pipeline::type() const noexcept
{
    return type_;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_pipeline::init_render_pipeline_state_(const Pipeline_desc<Pipeline_type::render>& desc)
{
    // configure a render pipeline descriptor.
    auto descriptor = [[MTLRenderPipelineDescriptor alloc] init];

    // configure vertex functions.
    {
        auto mtl_shader = static_cast<Mtl_shader*>(desc.vertex_shader_stage);

        assert(mtl_shader);
        descriptor.vertexFunction = mtl_shader->function();
    }

    // configure fragment functions.
    {
        auto mtl_shader = static_cast<Mtl_shader*>(desc.fragment_shader_stage);

        assert(mtl_shader);
        descriptor.fragmentFunction = mtl_shader->function();
    }

    // configure buffer layouts and fetch behavior.
    {
        auto& vertex_state = desc.vertex_state;

        for (auto i = 0; i != vertex_state.bindings.size(); ++i) {
            auto& binding = vertex_state.bindings[i];

            if (UINT32_MAX == binding.stride)
                continue;

            auto index = i + vertex_buffer_index_offset;

            descriptor.vertexDescriptor.layouts[index].stride = binding.stride;
            descriptor.vertexDescriptor.layouts[index].stepRate = 1;
            descriptor.vertexDescriptor.layouts[index].stepFunction = convert(binding.step_rate);
        }

        for (auto i = 0; i != vertex_state.attributes.size(); ++i ){
            auto& attribute = vertex_state.attributes[i];

            if (UINT32_MAX == attribute.binding)
                continue;

            descriptor.vertexDescriptor.attributes[i].format = convert<MTLVertexFormat>(attribute.format);
            descriptor.vertexDescriptor.attributes[i].offset = attribute.offset;
            descriptor.vertexDescriptor.attributes[i].bufferIndex = attribute.binding + vertex_buffer_index_offset;
        }
    }

    // configure the multisample state.
    {
        auto& multisample_stage = desc.multisample_stage;

        descriptor.sampleCount = multisample_stage.samples;
    }

    // configure rendering pipeline state.
    {
        auto& color_blend_stage = desc.color_blend_stage;
        auto& output_merger_stage = desc.output_merger_stage;

        for (auto i = 0; i != 4; ++i) {
            auto& color_format = output_merger_stage.color_formats[i];
            auto& attachment = color_blend_stage.attachments[i];

            if (Format::invalid == color_format)
                continue;

            descriptor.colorAttachments[i].pixelFormat = convert<MTLPixelFormat>(color_format);
            descriptor.colorAttachments[i].writeMask = attachment.write_mask;
            descriptor.colorAttachments[i].blendingEnabled = attachment.enable_blend;
            descriptor.colorAttachments[i].sourceRGBBlendFactor = convert(attachment.src_rgb_blend_factor);
            descriptor.colorAttachments[i].destinationRGBBlendFactor = convert(attachment.dst_rgb_blend_factor);
            descriptor.colorAttachments[i].rgbBlendOperation = convert(attachment.rgb_blend_op);
            descriptor.colorAttachments[i].sourceAlphaBlendFactor = convert(attachment.src_alpha_blend_factor);
            descriptor.colorAttachments[i].destinationAlphaBlendFactor = convert(attachment.dst_alpha_blend_factor);
            descriptor.colorAttachments[i].alphaBlendOperation = convert(attachment.alpha_blend_op);
        }

        auto& depth_stencil_format = output_merger_stage.depth_stencil;

        if (Format::invalid != depth_stencil_format) {
            descriptor.depthAttachmentPixelFormat = convert<MTLPixelFormat>(depth_stencil_format);
            descriptor.stencilAttachmentPixelFormat = convert<MTLPixelFormat>(depth_stencil_format);
        }
    }

    // configure rasterization and visibility state.
    {
        auto& input_assembly_stage = desc.input_assembly_stage;

        descriptor.inputPrimitiveTopology = convert<MTLPrimitiveTopologyClass>(input_assembly_stage.topology);
    }

    // try to create a render pipeline state.
    render_pipeline_state_ = [device_->device() newRenderPipelineStateWithDescriptor:descriptor
                                                                               error:nil];

    if (!render_pipeline_state_)
        throw runtime_error("fail to create the pipeline");
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_pipeline::init_depth_stencil_state_(const Depth_stencil_stage& stage)
{
    // configure the depth stencil descriptor.
    auto descriptor = [[MTLDepthStencilDescriptor alloc] init];

    if (stage.enable_depth_test) {
        descriptor.depthCompareFunction = convert(stage.depth_compare_op);
        descriptor.depthWriteEnabled = stage.enable_depth_write;
    }

    if (stage.enable_stencil_test) {
        auto& front_stencil_state = stage.front_stencil_state;

        descriptor.frontFaceStencil.stencilFailureOperation = convert(front_stencil_state.stencil_fail_op);
        descriptor.frontFaceStencil.depthFailureOperation = convert(front_stencil_state.depth_fail_op );
        descriptor.frontFaceStencil.depthStencilPassOperation = convert(front_stencil_state.depth_stencil_pass_op);
        descriptor.frontFaceStencil.stencilCompareFunction = convert(front_stencil_state.compare_op);
        descriptor.frontFaceStencil.readMask = front_stencil_state.read_mask;
        descriptor.frontFaceStencil.writeMask = front_stencil_state.write_mask;

        auto& back_stencil_state = stage.back_stencil_state;

        descriptor.backFaceStencil.stencilFailureOperation = convert(back_stencil_state.stencil_fail_op);
        descriptor.backFaceStencil.depthFailureOperation = convert(back_stencil_state.depth_fail_op );
        descriptor.backFaceStencil.depthStencilPassOperation = convert(back_stencil_state.depth_stencil_pass_op);
        descriptor.backFaceStencil.stencilCompareFunction = convert(back_stencil_state.compare_op);
        descriptor.backFaceStencil.readMask = back_stencil_state.read_mask;
        descriptor.backFaceStencil.writeMask = back_stencil_state.write_mask;
    }

    // try to create a depth stencil state.
    depth_stencil_state_ = [device_->device() newDepthStencilStateWithDescriptor:descriptor];

    if (!depth_stencil_state_)
        throw runtime_error("fail to create the pipeline");
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
