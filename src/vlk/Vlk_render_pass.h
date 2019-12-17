//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_VLK_RENDER_PASS_GUARD
#define GFX_VLK_RENDER_PASS_GUARD

#include <array>
#include <vulkan/vulkan.h>

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Vlk_device;

//----------------------------------------------------------------------------------------------------------------------

struct Vlk_attachment {
    Format format { Format::invalid };
    uint8_t samples { 0 };
    Load_op load_op { Load_op::dont_care };
    Store_op store_op { Store_op::dont_care };
};

//----------------------------------------------------------------------------------------------------------------------

struct Vlk_render_pass_desc {
    std::array<Vlk_attachment, 4> colors;
    Vlk_attachment depth_stencil;
};

//----------------------------------------------------------------------------------------------------------------------

class Vlk_render_pass final {
public:
    Vlk_render_pass(const Vlk_render_pass_desc& desc, Vlk_device* device);

    ~Vlk_render_pass();

    inline auto& render_pass() const noexcept
    { return render_pass_; }

private:
    void init_render_pass_(const Vlk_render_pass_desc& desc);

    void fini_render_pass_();

private:
    Vlk_device* device_;
    VkRenderPass render_pass_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_VLK_RENDER_PASS_GUARD
