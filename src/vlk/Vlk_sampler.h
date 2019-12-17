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

    Filter min() const noexcept override;

    Filter mag() const noexcept override;

    Mip_filter mip() const noexcept override;

    Address_mode u() const noexcept override;

    Address_mode v() const noexcept override;

    Address_mode w() const noexcept override;

    inline auto& sampler() const noexcept
    { return sampler_; }

private:
    void init_sampler_(const Sampler_desc& desc);

    void fini_sampler_();

private:
    Vlk_device* device_;
    Filter min_;
    Filter mag_;
    Mip_filter mip_;
    Address_mode u_;
    Address_mode v_;
    Address_mode w_;
    VkSampler sampler_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_VLK_SAMPLER_GUARD
