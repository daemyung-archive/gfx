//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include <array>
#include <sc/Spirv_reflector.h>
#include <sc/Glsl_compiler.h>
#include "ogl_lib.h"
#include "Ogl_shader.h"
#include "Ogl_device.h"

using namespace std;
using namespace Sc_lib;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Ogl_shader::Ogl_shader(const Shader_desc& desc, Ogl_device* device) :
    Shader {desc},
    device_ {device},
    signature_ {},
    shader_ {0}
{
    init_signature_(desc.src);
    init_shader_(Glsl_compiler().compile(desc.src));
}

//----------------------------------------------------------------------------------------------------------------------

Ogl_shader::~Ogl_shader()
{
    fini_shader_();
}

//----------------------------------------------------------------------------------------------------------------------

Device* Ogl_shader::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

Sc_lib::Signature Ogl_shader::reflect() const noexcept
{
    return signature_;
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_shader::init_signature_(const std::vector<uint32_t>& src)
{
    signature_ = Spirv_reflector().reflect(src);
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_shader::init_shader_(const std::string& src)
{
    shader_ = glCreateShader(to_GLShaderType(type_));

    auto contents = src.c_str();
    auto length = static_cast<GLint>(src.size());

    glShaderSource(shader_, 1, &contents, &length);
    glCompileShader(shader_);
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_shader::fini_shader_()
{
    glDeleteShader(shader_);
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

