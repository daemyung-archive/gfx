//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_OGL_FRAMEBUFFER_GUARD
#define GFX_OGL_FRAMEBUFFER_GUARD

#include <array>
#include <GLES3/gl3.h>
#include "types.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Ogl_device;
class Ogl_image;

//----------------------------------------------------------------------------------------------------------------------

struct Ogl_framebuffer_desc {
    std::array<Ogl_image*, 4> colors;
    Ogl_image* depth_stencil;
};

//----------------------------------------------------------------------------------------------------------------------

class Ogl_framebuffer final {
public:
    Ogl_framebuffer(const Ogl_framebuffer_desc& desc, Ogl_device* device);

    ~Ogl_framebuffer();

    inline auto extent() const noexcept
    { return extent_; }

    inline auto color_image(uint64_t index) const noexcept
    { return colors_[index]; }

    inline auto depth_stencil() const noexcept
    { return depth_stencil_; }

    inline auto framebuffer() const noexcept
    { return framebuffer_; }

private:
    void init_extent_();

    void init_framebuffer_();

    void fini_framebuffer_();

private:
    Ogl_device* device_;
    std::array<Ogl_image*, 4> colors_;
    Ogl_image* depth_stencil_;
    Extent extent_;
    GLuint framebuffer_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_VLK_FRAMEBUFFER_GUARD
