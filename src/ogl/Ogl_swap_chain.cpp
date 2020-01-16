//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "Ogl_swap_chain.h"
#include "Ogl_device.h"
#include "Ogl_image.h"

using namespace std;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Ogl_swap_chain::Ogl_swap_chain(const Swap_chain_desc& desc, Ogl_device* device) :
    Swap_chain {desc},
    device_ {device},
    surface_ {EGL_NO_SURFACE},
    image_index_ {0},
    images_ {desc.image_count}
{
    init_surface_(desc.window);
    init_images_();
}

//----------------------------------------------------------------------------------------------------------------------

Ogl_swap_chain::~Ogl_swap_chain()
{
    fini_surface_();
}

//----------------------------------------------------------------------------------------------------------------------

Image* Ogl_swap_chain::acquire()
{
    return images_[image_index_].get();
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_swap_chain::present()
{
    eglSwapBuffers(device_->display(), surface_);
    image_index_ = ++frame_count_ % images_.size();
}

//----------------------------------------------------------------------------------------------------------------------

Device* Ogl_swap_chain::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_swap_chain::init_surface_(void* window)
{
    constexpr EGLint attributes[] {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE,    8,
        EGL_GREEN_SIZE,   8,
        EGL_RED_SIZE,     8,
        EGL_NONE
    };

    EGLint config_count;

    eglChooseConfig(device_->display(), attributes, nullptr, 0, &config_count);

    EGLConfig config;

    eglChooseConfig(device_->display(), attributes, &config, 1, &config_count);

    surface_ = eglCreateWindowSurface(device_->display(), config,
                                      static_cast<EGLNativeWindowType>(window), nullptr);

    if (!surface_)
        throw runtime_error("fail to create a swap chain");

    eglMakeCurrent(device_->display(), surface_, surface_, device_->context());
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_swap_chain::init_images_()
{
    Image_desc desc;

    desc.type = Image_type::swap_chain;
    desc.format = image_format_;
    desc.extent = image_extent_;

    for (auto& image : images_) {
        try {
            image = make_unique<Ogl_image>(desc, device_);
        }
        catch (exception& e) {
            throw runtime_error("fail to create a swap chain");
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_swap_chain::fini_surface_()
{
    eglMakeCurrent(device_->display(), EGL_NO_SURFACE, EGL_NO_SURFACE, device_->context());
    eglDestroySurface(device_->display(), surface_);
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib