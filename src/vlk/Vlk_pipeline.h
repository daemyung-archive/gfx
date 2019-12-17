//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_VLK_PIPELINE_GUARD
#define GFX_VLK_PIPELINE_GUARD

#include <vulkan/vulkan.h>
#include "Pipeline.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Vlk_device;

//----------------------------------------------------------------------------------------------------------------------

class Vlk_pipeline final : public Pipeline {
public:
    Vlk_pipeline(const Pipeline_desc<Pipeline_type::render>& desc, Vlk_device* device);

    Device* device() const override;

    Pipeline_type type() const noexcept override;

    inline auto& pipeline() const noexcept
    { return pipeline_; }

private:
    void init_render_pipeline(const Pipeline_desc<Pipeline_type::render>& desc);

private:
    Vlk_device* device_;
    Pipeline_type type_;
    VkPipeline pipeline_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_VLK_PIPELINE_GUARD
