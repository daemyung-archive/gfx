//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_VLK_PIPELINE_GUARD
#define GFX_VLK_PIPELINE_GUARD

#include <memory>
#include <array>
#include <unordered_map>
#include <vulkan/vulkan.h>
#include "Pipeline.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Vlk_device;
class Vlk_set_layout;

//----------------------------------------------------------------------------------------------------------------------

using Vlk_set_layout_array = std::array<std::unique_ptr<Vlk_set_layout>, 2>;

//----------------------------------------------------------------------------------------------------------------------

class Vlk_pipeline final : public Pipeline {
public:
    Vlk_pipeline(const Pipeline_desc& desc, Vlk_device* device);

    ~Vlk_pipeline() override;

    Device* device() const override;

    inline auto set_layout(uint32_t index) const noexcept
    { return set_layouts_[index].get(); }

    inline auto& pipeline_layout() const noexcept
    { return pipeline_layout_; }

    inline auto& pipeline() const noexcept
    { return pipeline_; }

private:
    void init_set_layouts_(Shader* vertex_shader, Shader* fragment_shader);

    void init_pipeline_layout_();

    void init_pipeline_(Shader* vertex_shader, Shader* fragment_shader);

    void fini_pipeline_layout_();

    void fini_pipeline_();

private:
    Vlk_device* device_;
    Vlk_set_layout_array set_layouts_;
    VkPipelineLayout pipeline_layout_;
    VkPipeline pipeline_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_VLK_PIPELINE_GUARD

