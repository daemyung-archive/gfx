//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_OGL_DEVICE_GUARD
#define GFX_OGL_DEVICE_GUARD

#include <EGL/egl.h>
#include "Device.h"
#include "Lru_cache.h"
#include "Ogl_framebuffer.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Ogl_device : public Device {
public:
    Ogl_device();

    ~Ogl_device() override;

    std::unique_ptr<Buffer> create(const Buffer_desc& desc) override;

    std::unique_ptr<Image> create(const Image_desc& desc) override;

    std::unique_ptr<Sampler> create(const Sampler_desc& desc) override;

    std::unique_ptr<Shader> create(const Shader_desc& desc) override;

    std::unique_ptr<Pipeline> create(const Pipeline_desc& desc) override;

    std::unique_ptr<Swap_chain> create(const Swap_chain_desc& desc) override;

    std::unique_ptr<Cmd_buffer> create(const Cmd_buffer_desc& desc) override;

    std::unique_ptr<Fence> create(const Fence_desc& desc) override;

    void submit(Cmd_buffer* cmd_buffer, Fence* fence = nullptr) override;

    void wait_idle() override;

    inline auto display() const noexcept
    { return display_; }

    inline auto context() const noexcept
    { return context_; }

    Ogl_framebuffer* framebuffer(const Ogl_framebuffer_desc& desc);

private:
    void init_display_();

    void init_context_();

    void init_context_symbols_();

    void init_caps_();

    void fini_context_();

private:
    EGLDisplay display_;
    EGLContext context_;
    Lru_cache<Ogl_framebuffer> framebuffer_pool_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_OGL_DEVICE_GUARD
