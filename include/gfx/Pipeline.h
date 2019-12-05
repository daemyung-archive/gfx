//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_PIPELINE_GUARD
#define GFX_PIPELINE_GUARD

#include <array>
// #include <sc/limits.h>
#include "gfx/enums.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Device;
class Shader;

//----------------------------------------------------------------------------------------------------------------------

struct Vertex_attribute_state {
    uint32_t binding { UINT32_MAX };
    Format format { Format::invalid };
    uint32_t offset { 0 };
};

//----------------------------------------------------------------------------------------------------------------------

struct Vertex_binding_state {
    uint32_t stride { UINT32_MAX };
    Step_rate step_rate { Step_rate::vertex };
};

//----------------------------------------------------------------------------------------------------------------------

struct Vertex_state {
    std::array<Vertex_attribute_state, 16> attributes;
    std::array<Vertex_binding_state, 2> bindings;
};

//----------------------------------------------------------------------------------------------------------------------

struct Input_assembly_stage {
    Topology topology { Topology::triangle_list };
    bool restart { false };
};

//----------------------------------------------------------------------------------------------------------------------

struct Rasterization_stage {
    bool enable_depth_clamp { false };
    Cull_mode cull_mode { Cull_mode::back };
    Front_face front_face { Front_face::counter_clockwise };
    bool enable_depth_bias { false };
    float depth_bias_constant_factor { 0.0f };
    float depth_bias_clamp { 1.0f };
    float depth_bias_slope_factor { 1.0f };
};

//----------------------------------------------------------------------------------------------------------------------

struct Multisample_stage {
    uint32_t samples = { 1 };
};

//----------------------------------------------------------------------------------------------------------------------

struct Stencil_state {
    Stencil_op stencil_fail_op;
    Stencil_op depth_fail_op;
    Stencil_op depth_stencil_pass_op;
    Compare_op compare_op;
    uint32_t read_mask { 0xFFFF };
    uint32_t write_mask { 0xFFFF };
    uint32_t referece { 0 };
};

//----------------------------------------------------------------------------------------------------------------------

struct Depth_stencil_stage {
    bool enable_depth_test { false };
    bool enable_depth_write { false };
    Compare_op depth_compare_op { Compare_op::less };
    bool enable_stencil_test { false };
    Stencil_state front_stencil_state;
    Stencil_state back_stencil_state;
};

//----------------------------------------------------------------------------------------------------------------------

struct Color_blend_attachment_state {
    bool enable_blend { false };
    Blend_factor src_rgb_blend_factor { Blend_factor::one };
    Blend_factor dst_rgb_blend_factor { Blend_factor::zero };
    Blend_op rgb_blend_op { Blend_op::add };
    Blend_factor src_alpha_blend_factor { Blend_factor::one };
    Blend_factor dst_alpha_blend_factor { Blend_factor::zero };
    Blend_op alpha_blend_op { Blend_op::add };
    uint32_t write_mask { 0xF };
};

//----------------------------------------------------------------------------------------------------------------------

struct Color_blend_stage {
    std::array<Color_blend_attachment_state, 4> attachments;
    std::array<float, 4> constant;
};

//----------------------------------------------------------------------------------------------------------------------

struct Output_merger_stage {
    std::array<Format, 4> color_formats { Format::invalid, Format::invalid, Format::invalid, Format::invalid };
    Format depth_stencil { Format::invalid };
};

//----------------------------------------------------------------------------------------------------------------------

template<Pipeline_type>
struct Pipeline_desc;

//----------------------------------------------------------------------------------------------------------------------

template<>
struct Pipeline_desc<Pipeline_type::render> {
    Vertex_state vertex_state;
    Input_assembly_stage input_assembly_stage;
    Shader* vertex_shader_stage;
    Rasterization_stage rasterization_stage;
    Shader* fragment_shader_stage;
    Multisample_stage multisample_stage;
    Depth_stencil_stage depth_stencil_stage;
    Color_blend_stage color_blend_stage;
    Output_merger_stage output_merger_stage;
};

//----------------------------------------------------------------------------------------------------------------------

template<>
struct Pipeline_desc<Pipeline_type::compute> {
};

//----------------------------------------------------------------------------------------------------------------------

class Pipeline {
public:
    virtual ~Pipeline() = default;

    virtual Device* device() const = 0;

    virtual Pipeline_type type() const noexcept = 0;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_PIPELINE_GUARD
