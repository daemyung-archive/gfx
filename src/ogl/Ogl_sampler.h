//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_OGL_SAMPLER_GUARD
#define GFX_OGL_SAMPLER_GUARD

#include <GLES3/gl3.h>
#include "Sampler.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Ogl_device;

//----------------------------------------------------------------------------------------------------------------------

class Ogl_sampler final : public Sampler {
public:
    Ogl_sampler(const Sampler_desc& desc, Ogl_device* device);

    ~Ogl_sampler() override;

    Device* device() const override;

    inline auto sampler() const noexcept
    { return sampler_; }

private:
    void init_sampler_();

    void fini_sampler_();

private:
    Ogl_device* device_;
    GLuint sampler_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_OGL_SAMPLER_GUARD
