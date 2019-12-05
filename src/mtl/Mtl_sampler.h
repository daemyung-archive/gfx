//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_MTL_SAMPLER_GUARD
#define GFX_MTL_SAMPLER_GUARD

#include <Metal/Metal.h>
#include "gfx/Sampler.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Mtl_device;

//----------------------------------------------------------------------------------------------------------------------

class Mtl_sampler final : public Sampler {
public:
    Mtl_sampler(const Sampler_desc& desc, Mtl_device* device);

    Device* device() const override;

    Filter min() const noexcept override;

    Filter mag() const noexcept override;

    Mip_filter mip() const noexcept override;

    Address_mode u() const noexcept override;

    Address_mode v() const noexcept override;

    Address_mode w() const noexcept override;

    inline auto sampler_state() const noexcept
    { return sampler_state_; }

private:
    void init_sampler_state_(const Sampler_desc& desc);

private:
    Mtl_device* device_;
    Filter min_;
    Filter mag_;
    Mip_filter mip_;
    Address_mode u_;
    Address_mode v_;
    Address_mode w_;
    id<MTLSamplerState> sampler_state_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_MTL_SAMPLER_GUARD
