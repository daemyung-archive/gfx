//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "std_lib.h"
#include "Pipeline.h"
#include "Shader.h"

using namespace std;
using namespace Sc_lib;
using namespace Gfx_lib;

namespace {

//----------------------------------------------------------------------------------------------------------------------

auto to_Pipeline_stage(Shader_type type)
{
    switch (type) {
        case Shader_type::vertex:
            return Pipeline_stage::vertex_shader;
        case Shader_type::fragment:
            return Pipeline_stage::fragment_shader;
        default:
            throw runtime_error("invalid Shader_type");
    }
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Pipeline::Pipeline(const Pipeline_desc& desc) :
    vertex_input_ {desc.vertex_input},
    input_assembly_ {desc.input_assembly},
    rasterization_ {desc.rasterization},
    multisample_ {desc.multisample},
    depth_stencil_ {desc.depth_stencil},
    color_blend_ {desc.color_blend},
    output_merger_ {desc.output_merger},
    reflection_ {}
{
    init_reflection_({desc.vertex_shader, desc.fragment_shader});
}

//----------------------------------------------------------------------------------------------------------------------

void Pipeline::init_reflection_(const std::vector<Shader*> shaders)
{
    unordered_map<Shader_type, Signature> signatures;

    for (auto shader : shaders)
        signatures.emplace(shader->type(), shader->reflect());

    for (auto& [shader_type, signature] : signatures) {
        for (auto& [binding, buffer] : signature.buffers) {
            try {
                reflection_.buffers[binding] |= etoi(to_Pipeline_stage(shader_type));
            }
            catch (exception& e) {
                throw runtime_error("fail to create a pipeline");
            }
        }

        for (auto& [binding, texture] : signature.textures) {
            try {
                reflection_.textures[binding] |= etoi(to_Pipeline_stage(shader_type));
            }
            catch (exception& e) {
                throw runtime_error("fail to create a pipeline");
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
