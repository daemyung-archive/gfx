
//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "mtl/Mtl_pipeline.h"
#include "mtl/Mtl_device.h"
#include "mtl/Mtl_shader.h"
#include "mtl/mtl_lib_modules.h"
#include "std_lib_modules.h"

using namespace std;
using namespace Gfx_lib;

namespace {

//----------------------------------------------------------------------------------------------------------------------

inline auto make(const Vertex_state& state)
{
    auto descriptor = [MTLVertexDescriptor vertexDescriptor];

    for (auto i = 0; i != state.bindings.size(); ++i) {
        auto& binding = state.bindings[i];

        if (UINT32_MAX == binding.stride)
            continue;

        descriptor.layouts[i].stride = binding.stride;
        descriptor.layouts[i].stepRate = 1;
        descriptor.layouts[i].stepFunction = convert(binding.step_rate);
    }

    for (auto i = 0; i != state.attributes.size(); ++i ){
        auto& attribute = state.attributes[i];

        if (UINT32_MAX == attribute.binding)
            continue;

        descriptor.attributes[i].format = convert<MTLVertexFormat>(attribute.format);
        descriptor.attributes[i].offset = attribute.offset;
        descriptor.attributes[i].bufferIndex = attribute.binding;
    }

    return descriptor;
}

//----------------------------------------------------------------------------------------------------------------------

inline auto make(const Stencil_state& state)
{
    auto descriptor = [[MTLStencilDescriptor alloc] init];

    descriptor.stencilFailureOperation = convert(state.stencil_fail_op );
    descriptor.depthFailureOperation = convert(state.depth_fail_op );
    descriptor.depthStencilPassOperation = convert(state.depth_stencil_pass_op);
    descriptor.stencilCompareFunction = convert(state.compare_op);
    descriptor.readMask = descriptor.readMask;
    descriptor.writeMask = descriptor.writeMask;

    return descriptor;
}

//----------------------------------------------------------------------------------------------------------------------

inline auto make(const Depth_stencil_stage& stage)
{
    auto  descriptor = [[MTLDepthStencilDescriptor alloc] init];

    if (stage.enable_depth_test) {
        descriptor.depthCompareFunction = convert(stage.depth_compare_op);
        descriptor.depthWriteEnabled = stage.enable_depth_write;
    }

    if (stage.enable_stencil_test) {
        descriptor.frontFaceStencil = make(stage.front_stencil_state);
        descriptor.backFaceStencil = make(stage.back_stencil_state);
    }

    return descriptor;
}

//----------------------------------------------------------------------------------------------------------------------

inline auto make(const Pipeline_desc<Pipeline_type::render>& desc)
{
    auto descriptor = [[MTLRenderPipelineDescriptor alloc] init];

    // Specifying vertex functions.
    {
        auto mtl_shader = static_cast<Mtl_shader*>(desc.vertex_shader_stage);

        assert(mtl_shader);
        descriptor.vertexFunction = mtl_shader->function();
    }

    // Specifying fragment functions.
    {
        auto mtl_shader = static_cast<Mtl_shader*>(desc.fragment_shader_stage);

        assert(mtl_shader);
        descriptor.fragmentFunction = mtl_shader->function();
    }

    // Specifying buffer layouts and fetch behavior.
    {
        auto& vertex_state = desc.vertex_state;

        descriptor.vertexDescriptor = make(vertex_state);
    }

    // Specifying the multisample state.
    {
        auto& multisample_stage = desc.multisample_stage;

        descriptor.sampleCount = multisample_stage.samples;
    }

    // Specifying rendering pipeline state.
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

    // Specifying rasterization and visibility state.
    {
        auto& input_assembly_stage = desc.input_assembly_stage;

        descriptor.inputPrimitiveTopology = convert<MTLPrimitiveTopologyClass>(input_assembly_stage.topology);
    }

    return descriptor;
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace

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
        init_depth_stencil_state_(desc);
}

//----------------------------------------------------------------------------------------------------------------------

Pipeline_type Mtl_pipeline::type() const noexcept
{
    return type_;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_pipeline::init_render_pipeline_state_(const Pipeline_desc<Pipeline_type::render>& desc)
{
    auto descriptor = make(desc);

    render_pipeline_state_ = [device_->device() newRenderPipelineStateWithDescriptor:descriptor
                                                                               error:nil];

    if (!render_pipeline_state_)
        throw runtime_error("fail to create the pipeline");
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_pipeline::init_depth_stencil_state_(const Pipeline_desc<Pipeline_type::render>& desc)
{
    auto descriptor = make(desc.depth_stencil_stage);

    assert(descriptor);
    depth_stencil_state_ = [device_->device() newDepthStencilStateWithDescriptor:descriptor];

    if (!depth_stencil_state_)
        throw runtime_error("fail to create the pipeline");
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
