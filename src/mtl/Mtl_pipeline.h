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
    Mtl_pipeline(const Pipeline_desc& desc, Mtl_device* device);

    Device* device() const override;

    inline auto render_pipeline_state() const noexcept
    { return render_pipeline_state_; }

    inline auto depth_stencil_state() const noexcept
    { return depth_stencil_state_; }

private:
    void init_render_pipeline_state_(Shader* vertex_shader, Shader* fragment_shader);

    void init_depth_stencil_state_();

private:
    Mtl_device* device_;
    id<MTLRenderPipelineState> render_pipeline_state_;
    id<MTLDepthStencilState> depth_stencil_state_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_MTL_PIPELINE_GUARD
