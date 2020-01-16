//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "ogl_lib.h"
#include "Ogl_image.h"
#include "Ogl_device.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Ogl_image::Ogl_image(const Image_desc& desc, Ogl_device* device) :
    Image {desc},
    device_ {device},
    texture_ {0}
{
    init_texture_();
}

//----------------------------------------------------------------------------------------------------------------------

Ogl_image::~Ogl_image()
{
    fini_texture_();
}

//----------------------------------------------------------------------------------------------------------------------

Device* Ogl_image::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_image::init_texture_()
{
    if (Image_type::two_dim == type_ || Image_type::cube == type_) {
        glGenTextures(1, &texture_);

        if (Image_type::two_dim == type_) {
            glBindTexture(GL_TEXTURE_2D, texture_);
            glTexStorage2D(GL_TEXTURE_2D, 1, to_GLInternalFormat(format_), extent_.w, extent_.h);
        }
        else {
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);

            for (auto i = 0; i != 6; ++i) {
                glTexStorage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                               1, to_GLInternalFormat(format_), extent_.w, extent_.h);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_image::fini_texture_()
{
    if (Image_type::two_dim == type_ || Image_type::cube == type_)
        glDeleteTextures(1, &texture_);
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace of Gfx_lib