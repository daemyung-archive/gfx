//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_PIPELINE_GUARD
#define GFX_PIPELINE_GUARD

#include <array>
#include <unordered_map>
#include "limitations.h"
#include "enums.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Device;
class Shader;

//----------------------------------------------------------------------------------------------------------------------

struct Vertex_input_attribute final {
    uint32_t binding {UINT32_MAX};
    Format format {Format::invalid};
    uint32_t offset {0};
};

//----------------------------------------------------------------------------------------------------------------------

struct Vertex_input_binding final {
    uint32_t stride {UINT32_MAX};
    Step_rate step_rate {Step_rate::vertex};
};

//----------------------------------------------------------------------------------------------------------------------

struct Vertex_input final {
    std::array<Vertex_input_attribute, max_vertex_input_attributes> attributes;
    std::array<Vertex_input_binding, max_vertex_input_bindings> bindings;
};

//----------------------------------------------------------------------------------------------------------------------

struct Input_assembly final {
    Topology topology {Topology::triangle_list};
    bool restart {false};
};

//----------------------------------------------------------------------------------------------------------------------

struct Rasterization final {
    bool depth_clamp {false};
    Cull_mode cull_mode {Cull_mode::back};
    Front_face front_face {Front_face::counter_clockwise};
    bool depth_bias {false};
    float depth_bias_constant_factor {0.0f};
    float depth_bias_clamp {1.0f};
    float depth_bias_slope_factor {1.0f};
};

//----------------------------------------------------------------------------------------------------------------------

struct Multisample final {
    uint8_t samples {1};
};

//----------------------------------------------------------------------------------------------------------------------

struct Stencil final {
    Stencil_op stencil_fail_op {Stencil_op::keep};
    Stencil_op depth_fail_op {Stencil_op::keep};
    Stencil_op depth_stencil_pass_op {Stencil_op::keep};
    Compare_op compare_op {Compare_op::always};
    uint32_t read_mask {0xFFFF};
    uint32_t write_mask {0xFFFF};
    uint32_t referece {0};
};

//----------------------------------------------------------------------------------------------------------------------

struct Depth_stencil final {
    bool depth_test {false};
    bool write_mask {true};
    Compare_op depth_compare_op {Compare_op::less};
    bool stencil_test {false};
    Stencil front_stencil;
    Stencil back_stencil;
};

//----------------------------------------------------------------------------------------------------------------------

struct Color_blend_attachment final {
    bool blend {false};
    Blend_factor src_rgb_blend_factor {Blend_factor::one};
    Blend_factor dst_rgb_blend_factor {Blend_factor::zero};
    Blend_op rgb_blend_op {Blend_op::add};
    Blend_factor src_a_blend_factor {Blend_factor::one};
    Blend_factor dst_a_blend_factor {Blend_factor::zero};
    Blend_op a_blend_op { Blend_op::add };
    uint32_t write_mask { 0xF };
};

//----------------------------------------------------------------------------------------------------------------------

struct Color_blend final {
    std::array<Color_blend_attachment, max_color_attachments> attachments;
    std::array<float, max_color_attachments> constant;
};

//----------------------------------------------------------------------------------------------------------------------

struct Output_merger final {
    std::array<Format, max_color_attachments> color_formats {
        Format::invalid, Format::invalid, Format::invalid, Format::invalid
    };
    Format depth_stencil_format {Format::invalid};
};

//----------------------------------------------------------------------------------------------------------------------

struct Reflection {
    std::unordered_map<uint32_t, uint32_t> buffers;
    std::unordered_map<uint32_t, uint32_t> textures;
};

//----------------------------------------------------------------------------------------------------------------------

struct Pipeline_desc final {
    Vertex_input vertex_input;
    Input_assembly input_assembly;
    Shader* vertex_shader;
    Rasterization rasterization;
    Shader* fragment_shader;
    Multisample multisample;
    Depth_stencil depth_stencil;
    Color_blend color_blend;
    Output_merger output_merger;
};

//----------------------------------------------------------------------------------------------------------------------

class Pipeline {
public:
    explicit Pipeline(const Pipeline_desc& desc);

    virtual ~Pipeline() = default;

    virtual Device* device() const = 0;

    inline auto vertex_input() const noexcept
    { return vertex_input_; }

    inline auto input_assembly() const noexcept
    { return input_assembly_; }

    inline auto rasterization() const noexcept
    { return rasterization_; }

    inline auto multisample() const noexcept
    { return multisample_; }

    inline auto depth_stencil() const noexcept
    { return depth_stencil_; }

    inline auto color_blend() const noexcept
    { return color_blend_; }

    inline auto output_merger() const noexcept
    { return output_merger_; }

    inline auto reflection() const noexcept
    { return reflection_; }

private:
    void init_reflection_(const std::vector<Shader*> shaders);

protected:
    Vertex_input vertex_input_;
    Input_assembly input_assembly_;
    Rasterization rasterization_;
    Multisample multisample_;
    Depth_stencil depth_stencil_;
    Color_blend color_blend_;
    Output_merger output_merger_;
    Reflection reflection_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_PIPELINE_GUARD
