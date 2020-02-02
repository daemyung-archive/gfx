//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "std_lib.h"
#include "vlk_lib.h"
#include "Vlk_pipeline.h"
#include "Vlk_device.h"
#include "Vlk_shader.h"
#include "Vlk_render_pass.h"
#include "Vlk_set_layout.h"

using namespace std;
using namespace Sc_lib;
using namespace Gfx_lib;

namespace {

//----------------------------------------------------------------------------------------------------------------------

inline auto to_VkShaderStageFlags(uint32_t stages)
{
    VkShaderStageFlags flags {0};

    if (stages & etoi(Pipeline_stage::vertex_shader))
        flags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;

    if (stages & etoi(Pipeline_stage::fragment_shader))
        flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    return flags;
}

//----------------------------------------------------------------------------------------------------------------------

inline auto to_VkPipelineShaderStageCreateInfo(Shader* shader)
{
    VkPipelineShaderStageCreateInfo create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    create_info.stage = to_VkShaderStageFlagBits(shader->type());
    create_info.module = static_cast<Vlk_shader*>(shader)->shader_module();
    create_info.pName = "main";

    return create_info;
}

//----------------------------------------------------------------------------------------------------------------------

inline auto to_VkStencilOpState(const Stencil& stencil)
{
    VkStencilOpState state {};

    state.failOp = to_VkStencilOp(stencil.stencil_fail_op);
    state.passOp = to_VkStencilOp(stencil.depth_stencil_pass_op);
    state.depthFailOp = to_VkStencilOp(stencil.depth_fail_op);
    state.compareOp = to_VkCompareOp(stencil.compare_op);
    state.compareMask = stencil.read_mask;
    state.writeMask = stencil.write_mask;
    state.reference = stencil.referece;

    return state;
}

//----------------------------------------------------------------------------------------------------------------------

inline auto to_Render_pass_desc(const Multisample &multisample, const Output_merger &output_merger)
{
    // configure a render pass desc.
    Vlk_render_pass_desc render_pass_desc {};

    for (auto i = 0; i != 4; ++i) {
        render_pass_desc.colors[i].format = output_merger.color_formats[i];
        render_pass_desc.colors[i].samples = multisample.samples;
    }

    render_pass_desc.depth_stencil.format = output_merger.depth_stencil_format;
    render_pass_desc.depth_stencil.samples = multisample.samples;

    return render_pass_desc;
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Vlk_pipeline::Vlk_pipeline(const Pipeline_desc& desc, Vlk_device* device) :
    Pipeline {desc},
    device_ {device},
    pipeline_layout_ {VK_NULL_HANDLE},
    pipeline_ {VK_NULL_HANDLE}
{
    init_set_layouts_(desc.vertex_shader, desc.fragment_shader);
    init_pipeline_layout_();
    init_pipeline_(desc.vertex_shader, desc.fragment_shader);
}

//----------------------------------------------------------------------------------------------------------------------

Vlk_pipeline::~Vlk_pipeline()
{
    fini_pipeline_();
    fini_pipeline_layout_();
}

//----------------------------------------------------------------------------------------------------------------------

Device* Vlk_pipeline::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_pipeline::init_set_layouts_(Shader* vertex_shader, Shader* fragment_shader)
{
    Vlk_set_layout_desc buffer_set_layout_desc;
    auto& buffer_bindings = buffer_set_layout_desc.bindings;

    for (auto& [index, stages] : reflection_.buffers) {
        VkDescriptorSetLayoutBinding binding {};

        binding.binding = index;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        binding.descriptorCount = 1;
        binding.stageFlags = to_VkShaderStageFlags(stages);

        buffer_bindings.emplace_back(binding);
    }

    if (!buffer_bindings.empty())
        set_layouts_[0] = make_unique<Vlk_set_layout>(buffer_set_layout_desc, device_);

    Vlk_set_layout_desc texture_set_layout_desc;
    auto& texture_bindings = texture_set_layout_desc.bindings;

    for (auto& [index, stages] : reflection_.textures) {
        VkDescriptorSetLayoutBinding binding {};

        binding.binding = index;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        binding.descriptorCount = 1;
        binding.stageFlags = to_VkShaderStageFlags(stages);

        texture_bindings.push_back(binding);
    }

    if (!texture_bindings.empty())
        set_layouts_[1] = make_unique<Vlk_set_layout>(texture_set_layout_desc, device_);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_pipeline::init_pipeline_layout_()
{
    std::vector<VkDescriptorSetLayout> desc_set_layouts;

    for (auto& set_layout : set_layouts_) {
        desc_set_layouts.push_back(set_layout->desc_set_layout());
    }

    VkPipelineLayoutCreateInfo create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    create_info.setLayoutCount = static_cast<uint32_t>(desc_set_layouts.size());
    create_info.pSetLayouts = &desc_set_layouts[0];

    if (vkCreatePipelineLayout(device_->device(), &create_info, nullptr, &pipeline_layout_))
        throw runtime_error("fail to create a pipeline.");
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_pipeline::init_pipeline_(Shader* vertex_shader, Shader* fragment_shader)
{
    // configure a vertex and a fragment shader stages.
    array<VkPipelineShaderStageCreateInfo, 2> shader_stages {
        to_VkPipelineShaderStageCreateInfo(vertex_shader),
        to_VkPipelineShaderStageCreateInfo(fragment_shader)
    };

    // configure vertex input bindings.
    vector<VkVertexInputBindingDescription> vertex_input_bindings;

    for (auto i = 0; i != 2; ++i) {
        if (UINT32_MAX == vertex_input_.bindings[i].stride)
            continue;

        VkVertexInputBindingDescription binding {};

        binding.binding = static_cast<uint32_t>(i);
        binding.stride = vertex_input_.bindings[i].stride;
        binding.inputRate = to_VkVertexInputRate(vertex_input_.bindings[i].step_rate);

        vertex_input_bindings.push_back(binding);
    }

    // configure vertex input attributes.
    vector<VkVertexInputAttributeDescription> vertex_input_attributes;

    for (auto i = 0; i != 16; ++i) {
        if (UINT32_MAX == vertex_input_.attributes[i].binding)
            continue;

        VkVertexInputAttributeDescription attribute {};

        attribute.location = static_cast<uint32_t>(i);
        attribute.binding = vertex_input_.attributes[i].binding;
        attribute.format = to_VkFormat(vertex_input_.attributes[i].format);
        attribute.offset = vertex_input_.attributes[i].offset;

        vertex_input_attributes.push_back(attribute);
    }

    // configure a vertex input stage.
    VkPipelineVertexInputStateCreateInfo vertex_input_state {};

    vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_input_bindings.size());
    vertex_input_state.pVertexBindingDescriptions = &vertex_input_bindings[0];
    vertex_input_state.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_input_attributes.size());
    vertex_input_state.pVertexAttributeDescriptions = &vertex_input_attributes[0];

    // configure an input assembly state.
    VkPipelineInputAssemblyStateCreateInfo input_assembly_state {};

    input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_state.topology = to_VkPrimitiveTopology(input_assembly_.topology);
    input_assembly_state.primitiveRestartEnable = static_cast<VkBool32>(input_assembly_.restart);

    // configure a viewport state create info.
    VkPipelineViewportStateCreateInfo viewport_state {};

    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    // configure a rasterization state.
    VkPipelineRasterizationStateCreateInfo rasterization_state {};

    rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_state.depthClampEnable = rasterization_.depth_clamp;
    rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_state.cullMode = to_VkCullModeFlags(rasterization_.cull_mode);
    rasterization_state.frontFace = to_VkFrontFace(rasterization_.front_face);
    rasterization_state.depthBiasEnable = rasterization_.depth_bias;
    rasterization_state.depthBiasConstantFactor = rasterization_.depth_bias_constant_factor;
    rasterization_state.depthBiasClamp = rasterization_.depth_bias_clamp;
    rasterization_state.depthBiasSlopeFactor = rasterization_.depth_bias_slope_factor;

    // configure a multisample state create info.
    VkPipelineMultisampleStateCreateInfo multisample_state {};

    multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_state.rasterizationSamples = to_VkSampleCountFlagBits(multisample_.samples);

    // configure a depth stencil state.
    VkPipelineDepthStencilStateCreateInfo depth_stencil_state {};

    depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil_state.depthTestEnable = depth_stencil_.depth_test;
    depth_stencil_state.depthWriteEnable = depth_stencil_.write_mask;
    depth_stencil_state.depthCompareOp = to_VkCompareOp(depth_stencil_.depth_compare_op);
    depth_stencil_state.stencilTestEnable = depth_stencil_.stencil_test;
    depth_stencil_state.front = to_VkStencilOpState(depth_stencil_.front_stencil);
    depth_stencil_state.back = to_VkStencilOpState(depth_stencil_.back_stencil);
    depth_stencil_state.minDepthBounds = 0.0f;
    depth_stencil_state.maxDepthBounds = 1.0f;

    // configure pipeline color blend attachment states.
    std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachment_states;

    for (auto i = 0; i != 4; ++i) {
        if (Format::invalid == output_merger_.color_formats[i])
            continue;

        auto& attachment = color_blend_.attachments[i];
        VkPipelineColorBlendAttachmentState attachment_state {};

        attachment_state.blendEnable = attachment.blend;
        attachment_state.srcColorBlendFactor = to_VkBlendFactor(attachment.src_rgb_blend_factor);
        attachment_state.dstColorBlendFactor = to_VkBlendFactor(attachment.dst_rgb_blend_factor);
        attachment_state.colorBlendOp = to_VkBlendOp(attachment.rgb_blend_op);
        attachment_state.srcAlphaBlendFactor = to_VkBlendFactor(attachment.src_a_blend_factor);
        attachment_state.dstAlphaBlendFactor = to_VkBlendFactor(attachment.dst_a_blend_factor);
        attachment_state.alphaBlendOp = to_VkBlendOp(attachment.a_blend_op);
        attachment_state.colorWriteMask = attachment.write_mask;

        color_blend_attachment_states.push_back(attachment_state);

    }

    // configure pipeline color blend state info.
    VkPipelineColorBlendStateCreateInfo color_blend_state {};

    color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_state.attachmentCount = color_blend_attachment_states.size();
    color_blend_state.pAttachments = &color_blend_attachment_states[0];
    color_blend_state.blendConstants[0] = color_blend_.constant[0];
    color_blend_state.blendConstants[1] = color_blend_.constant[1];
    color_blend_state.blendConstants[2] = color_blend_.constant[2];
    color_blend_state.blendConstants[3] = color_blend_.constant[3];

    constexpr array<VkDynamicState, 4> dynamic_states {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH,
        VK_DYNAMIC_STATE_DEPTH_BIAS
    };

    // configure dynamic state create info.
    VkPipelineDynamicStateCreateInfo dynamic_state {};

    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = dynamic_states.size();
    dynamic_state.pDynamicStates = &dynamic_states[0];

    // configure a graphics pipeline create info.
    VkGraphicsPipelineCreateInfo create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    create_info.stageCount = shader_stages.size();
    create_info.pStages = &shader_stages[0];
    create_info.pVertexInputState = &vertex_input_state;
    create_info.pInputAssemblyState = &input_assembly_state;
    create_info.pViewportState = &viewport_state;
    create_info.pRasterizationState= &rasterization_state;
    create_info.pMultisampleState = &multisample_state;
    create_info.pDepthStencilState = &depth_stencil_state;
    create_info.pColorBlendState = &color_blend_state;
    create_info.pDynamicState = &dynamic_state;
    create_info.layout = pipeline_layout_;
    create_info.renderPass = device_->render_pass(to_Render_pass_desc(multisample_, output_merger_))->render_pass();

    // try to create a graphics pipeline.
    if (vkCreateGraphicsPipelines(device_->device(), device_->pipeline_cache(), 1, &create_info, nullptr, &pipeline_))
        throw runtime_error("fail to create a pipeline.");
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_pipeline::fini_pipeline_layout_()
{
    vkDestroyPipelineLayout(device_->device(), pipeline_layout_, nullptr);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_pipeline::fini_pipeline_()
{
    vkDestroyPipeline(device_->device(), pipeline_, nullptr);
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
