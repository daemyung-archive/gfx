//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_MTL_PIPELINE_GUARD
#define GFX_MTL_PIPELINE_GUARD

#include <utility>
#include <Metal/Metal.h>
#include "Pipeline.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Mtl_device;

//----------------------------------------------------------------------------------------------------------------------

class Mtl_pipeline final : public Pipeline {
public:
    Mtl_pipeline(const Pipeline_desc<Pipeline_type::render>& desc, Mtl_device* device);

    Device* device() const override;

    Pipeline_type type() const noexcept override;

    inline auto primitive_type() const noexcept
    { return primitive_type_; }

    inline auto cull_mode() const noexcept
    { return cull_mode_; }

    inline auto depth_test_enabled() const noexcept
    { return depth_test_enabled_; }

    inline auto stencil_test_enabled() const noexcept
    { return stencil_test_enabled_; }

    inline auto front_stencil_reference() const noexcept
    { return front_stencil_reference_; }

    inline auto back_stencil_reference() const noexcept
    { return back_stencil_reference_; }

    inline auto render_pipeline_state() const noexcept
    { return render_pipeline_state_; }

    inline auto depth_stencil_state() const noexcept
    { return depth_stencil_state_; }

private:
    void init_render_pipeline_state_(const Pipeline_desc<Pipeline_type::render>& desc);

    void init_depth_stencil_state_(const Pipeline_desc<Pipeline_type::render>& desc);

private:
    Mtl_device* device_;
    Pipeline_type type_;
    MTLPrimitiveType primitive_type_;
    MTLCullMode cull_mode_;
    bool depth_test_enabled_;
    bool stencil_test_enabled_;
    uint32_t front_stencil_reference_;
    uint32_t back_stencil_reference_;
    id<MTLRenderPipelineState> render_pipeline_state_;
    id<MTLDepthStencilState> depth_stencil_state_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_MTL_PIPELINE_GUARD
