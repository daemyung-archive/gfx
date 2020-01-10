//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_VLK_SAMPLER_GUARD
#define GFX_VLK_SAMPLER_GUARD

#include <vulkan/vulkan.h>
#include "Sampler.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Vlk_device;

//----------------------------------------------------------------------------------------------------------------------

class Vlk_sampler final : public Sampler {
public:
    Vlk_sampler(const Sampler_desc& desc, Vlk_device* device);

    ~Vlk_sampler() override;

    Device* device() const override;

    inline auto& sampler() const noexcept
    { return sampler_; }

private:
    void init_sampler_();

    void fini_sampler_();

private:
    Vlk_device* device_;
    VkSampler sampler_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_VLK_SAMPLER_GUARD
