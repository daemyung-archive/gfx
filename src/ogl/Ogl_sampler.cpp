//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "ogl_lib.h"
#include "Ogl_sampler.h"
#include "Ogl_device.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Ogl_sampler::Ogl_sampler(const Sampler_desc& desc, Ogl_device* device) :
    Sampler {desc},
    device_ {device},
    sampler_ {0}
{
    init_sampler_();
}

//----------------------------------------------------------------------------------------------------------------------

Ogl_sampler::~Ogl_sampler()
{
    fini_sampler_();
}

//----------------------------------------------------------------------------------------------------------------------

Device* Ogl_sampler::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_sampler::init_sampler_()
{
    glGenSamplers(1, &sampler_);
    glSamplerParameteri(sampler_, GL_TEXTURE_MIN_LOD, 0);
    glSamplerParameteri(sampler_, GL_TEXTURE_MIN_FILTER, to_GLSamplerParam(min_, mip_));
    glSamplerParameteri(sampler_, GL_TEXTURE_MAG_FILTER, to_GLSamplerParam(mag_));
    glSamplerParameteri(sampler_, GL_TEXTURE_WRAP_S, to_GLSamplerWrapMode(u_));
    glSamplerParameteri(sampler_, GL_TEXTURE_WRAP_T, to_GLSamplerWrapMode(v_));
    // glSamplerParameteri(sampler_, GL_TEXTURE_WRAP_R, to_GLSamplerWrapMode(w_));
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_sampler::fini_sampler_()
{
    glDeleteSamplers(1, &sampler_);
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib