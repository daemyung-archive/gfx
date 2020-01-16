//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_OGL_SHADER_GUARD
#define GFX_OGL_SHADER_GUARD

#include <GLES3/gl3.h>
#include "Shader.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Ogl_device;

//----------------------------------------------------------------------------------------------------------------------

class Ogl_shader final : public Shader {
public:
    Ogl_shader(const Shader_desc& desc, Ogl_device* device);

    ~Ogl_shader() override;

    Device* device() const override;

    Sc_lib::Signature reflect() const noexcept override;

    inline auto shader() const noexcept
    { return shader_; }

private:
    void init_signature_(const std::vector<uint32_t>& src);

    void init_shader_(const std::string& src);

    void fini_shader_();

private:
    Ogl_device* device_;
    Sc_lib::Signature signature_;
    GLuint shader_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_OGL_SHADER_GUARD
