//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_OGL_PIPELINE_GUARD
#define GFX_OGL_PIPELINE_GUARD

#include <GLES3/gl3.h>
#include "Pipeline.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Ogl_device;

//----------------------------------------------------------------------------------------------------------------------

class Ogl_pipeline final : public Pipeline {
public:
    Ogl_pipeline(const Pipeline_desc& desc, Ogl_device* device);

    ~Ogl_pipeline() override;

    Device* device() const override;

    inline auto program() const noexcept
    { return program_; }

private:
    void init_program_(Shader* vertex_shader, Shader* fragment_shader);

    void fini_program_();

private:
    Ogl_device* device_;
    GLuint program_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_OGL_PIPELINE_GUARD

