//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include <metrohash.h>
#include "ogl_lib.h"
#include "Ogl_device.h"
#include "Ogl_buffer.h"
#include "Ogl_image.h"
#include "Ogl_sampler.h"
#include "Ogl_shader.h"
#include "Ogl_pipeline.h"
#include "Ogl_swap_chain.h"
#include "Ogl_cmd_buffer.h"
#include "Ogl_fence.h"

using namespace std;

#define DEFINE_OGL_SYMBOL(name) PFN_##name name;
#define LOAD_OGL_CONTEXT_SYMBOL(name) name = reinterpret_cast<PFN_##name>(eglGetProcAddress(#name));

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

APPLY_OGL_DRAW_BUFFERS_INDEXED_SYMBOLS(DEFINE_OGL_SYMBOL);

//----------------------------------------------------------------------------------------------------------------------

Ogl_device::Ogl_device()
    : Device {}
    , display_ {EGL_NO_DISPLAY}
    , context_ {EGL_NO_CONTEXT}
{
    init_display_();
    init_context_();
    init_context_symbols_();
    init_caps_();
}

//----------------------------------------------------------------------------------------------------------------------

Ogl_device::~Ogl_device()
{
    fini_context_();
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Buffer> Ogl_device::create(const Buffer_desc& desc)
{
    return make_unique<Ogl_buffer>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Image> Ogl_device::create(const Image_desc& desc)
{
    return make_unique<Ogl_image>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Sampler> Ogl_device::create(const Sampler_desc& desc)
{
    return make_unique<Ogl_sampler>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Shader> Ogl_device::create(const Shader_desc& desc)
{
    return make_unique<Ogl_shader>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Pipeline> Ogl_device::create(const Pipeline_desc& desc)
{
    return make_unique<Ogl_pipeline>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Swap_chain> Ogl_device::create(const Swap_chain_desc& desc)
{
    return make_unique<Ogl_swap_chain>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Cmd_buffer> Ogl_device::create(const Cmd_buffer_desc& desc)
{
    return make_unique<Ogl_cmd_buffer>(this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Fence> Ogl_device::create(const Fence_desc& desc)
{
    return make_unique<Ogl_fence>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_device::submit(Cmd_buffer* cmd_buffer, Fence* fence)
{
    glFlush();
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_device::wait_idle()
{
    glFinish();
}

//----------------------------------------------------------------------------------------------------------------------

Ogl_framebuffer* Ogl_device::framebuffer(const Ogl_framebuffer_desc& desc)
{
    // calculate a hash value.
    uint64_t key { 0 };

    MetroHash64::Hash(reinterpret_cast<const uint8_t*>(&desc), sizeof(Ogl_framebuffer_desc),
                      reinterpret_cast<uint8_t*>(&key));

    // check a framebuffer exists and if not then create it.
    if (!framebuffer_pool_.contains(key))
        framebuffer_pool_.emplace(key, desc, this);

    return *framebuffer_pool_.find(key);
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_device::init_display_()
{
    display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (EGL_NO_DISPLAY == display_)
        throw runtime_error("fail to create a device");

    eglInitialize(display_, nullptr, nullptr);
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_device::init_context_()
{
    constexpr EGLint attributes[] {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };

    context_ = eglCreateContext(display_, 0 ,EGL_NO_CONTEXT, attributes);

    if (EGL_NO_CONTEXT == context_)
        throw runtime_error("fail to create a device");

    eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, context_);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_device::init_context_symbols_()
{
    APPLY_OGL_DRAW_BUFFERS_INDEXED_SYMBOLS(LOAD_OGL_CONTEXT_SYMBOL);
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_device::init_caps_()
{
    caps_.window_coords = Coords::origin_lower_left;
    caps_.texture_coords = Coords::origin_lower_left;
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_device::fini_context_()
{
    eglDestroyContext(display_, context_);
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib