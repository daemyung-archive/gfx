//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "ogl_lib.h"
#include "Ogl_pipeline.h"
#include "Ogl_device.h"
#include "Ogl_shader.h"

using namespace std;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Ogl_pipeline::Ogl_pipeline(const Pipeline_desc& desc, Ogl_device* device) :
    Pipeline {desc},
    device_ {device},
    program_ {0}
{
    init_program_(desc.vertex_shader, desc.fragment_shader);
}

//----------------------------------------------------------------------------------------------------------------------

Ogl_pipeline::~Ogl_pipeline()
{
    fini_program_();
}

//----------------------------------------------------------------------------------------------------------------------

Device* Ogl_pipeline::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_pipeline::init_program_(Shader* vertex_shader, Shader* fragment_shader)
{
    program_ = glCreateProgram();

    if (!program_)
        throw runtime_error("fail to create a pipeline");

    glAttachShader(program_, static_cast<Ogl_shader*>(vertex_shader)->shader());
    glAttachShader(program_, static_cast<Ogl_shader*>(fragment_shader)->shader());
    glLinkProgram(program_);
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_pipeline::fini_program_()
{
    glDeleteProgram(program_);
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib