//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_VLK_FRAMEBUFFER_GUARD
#define GFX_VLK_FRAMEBUFFER_GUARD

#include <array>
#include <vulkan/vulkan.h>

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Vlk_device;
class Vlk_image;
class Vlk_render_pass;

//----------------------------------------------------------------------------------------------------------------------

struct Vlk_framebuffer_desc {
    Vlk_render_pass* render_pass;
    std::array<Vlk_image*, 4> colors;
    Vlk_image* depth_stencil;
};

//----------------------------------------------------------------------------------------------------------------------

class Vlk_framebuffer final {
public:
    Vlk_framebuffer(const Vlk_framebuffer_desc& desc, Vlk_device* device);

    ~Vlk_framebuffer();

    inline auto& extent() const noexcept
    { return extent_; }

    inline auto& framebuffer() const noexcept
    { return framebuffer_; }

private:
    void init_extent_(const Vlk_framebuffer_desc& desc);

    void init_framebuffer_(const Vlk_framebuffer_desc& desc);

    void fini_framebuffer_();

private:
    Vlk_device* device_;
    Extent extent_;
    VkFramebuffer framebuffer_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_VLK_FRAMEBUFFER_GUARD
