//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "std_lib.h"
#include "ogl_lib.h"
#include "Ogl_framebuffer.h"
#include "Ogl_image.h"

using namespace std;
using namespace Gfx_lib;

namespace {

//----------------------------------------------------------------------------------------------------------------------

inline bool is_swap_chain_image(Ogl_image* image)
{
    if (!image)
        return false;

    return Image_type::swap_chain == image->type();
}

//----------------------------------------------------------------------------------------------------------------------

}

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Ogl_framebuffer::Ogl_framebuffer(const Ogl_framebuffer_desc& desc, Ogl_device* device) :
    device_ {device},
    extent_ {0, 0, 1},
    colors_ {desc.colors},
    depth_stencil_ {desc.depth_stencil},
    framebuffer_ {0}
{
    init_extent_();
    init_framebuffer_();
}

//----------------------------------------------------------------------------------------------------------------------

Ogl_framebuffer::~Ogl_framebuffer()
{
    fini_framebuffer_();
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_framebuffer::init_extent_()
{
    extent_ = colors_[0]->extent();

    // check all attachments has a same extent.
    for (auto i = 1; i != 4; ++i) {
        auto& color = colors_[i];

        if (!color)
            continue;

        if (color->extent() != extent_)
            throw runtime_error("fail to create a framebuffer");
    }

    if (depth_stencil_) {
        if (depth_stencil_->extent() != extent_)
            throw runtime_error("fail to create a framebuffer");
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_framebuffer::init_framebuffer_()
{
    if (end(colors_) != find_if(colors_, is_swap_chain_image))
        return;

    glGenFramebuffers(1, &framebuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

    for (auto i = 0; i != 4; ++i) {
        auto& color = colors_[i];

        if (!color)
            continue;

        glBindTexture(GL_TEXTURE_2D, color->texture());
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0 + i,
                               GL_TEXTURE_2D,
                               color->texture(),
                               0);
    }

    if (depth_stencil_) {
        glBindTexture(GL_TEXTURE_2D, depth_stencil_->texture());
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D,
                               depth_stencil_->texture(),
                               0);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_STENCIL_ATTACHMENT,
                               GL_TEXTURE_2D,
                               depth_stencil_->texture(),
                               0);
    }

    if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
        throw runtime_error("fail to create a framebuffer");
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_framebuffer::fini_framebuffer_()
{
    if (framebuffer_)
        glDeleteFramebuffers(1, &framebuffer_);
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib