//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_OGL_IMAGE_GUARD
#define GFX_OGL_IMAGE_GUARD

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include "gfx/Image.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Ogl_device;

//----------------------------------------------------------------------------------------------------------------------

class Ogl_image final : public Image {
public:
    Ogl_image(const Image_desc& desc, Ogl_device* device);

    ~Ogl_image() override;

    Device* device() const override;

    inline auto texture() const noexcept
    { return texture_; }

private:
    void init_texture_();

    void fini_texture_();

private:
    Ogl_device* device_;
    GLuint texture_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_OGL_IMAGE_GUARD


