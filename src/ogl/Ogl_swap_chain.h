//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_OGL_SWAP_CHAIN_GUARD
#define GFX_OGL_SWAP_CHAIN_GUARD

#include <memory>
#include <vector>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include "Swap_chain.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Ogl_device;
class Ogl_image;

//----------------------------------------------------------------------------------------------------------------------

class Ogl_swap_chain final : public Swap_chain {
public:
    Ogl_swap_chain(const Swap_chain_desc& desc, Ogl_device* device);

    ~Ogl_swap_chain() override;

    Image* acquire() override;

    void present() override;

    Device* device() const override;

    inline auto surface() const noexcept
    { return surface_; }

private:
    void init_surface_(void* window);

    void init_images_();

    void fini_surface_();

private:
    Ogl_device* device_;
    EGLSurface surface_;
    uint64_t image_index_;
    std::vector<std::unique_ptr<Ogl_image>> images_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_OGL_SWAP_CHAIN_GUARD
