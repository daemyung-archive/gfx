//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_MTL_SAMPLER_GUARD
#define GFX_MTL_SAMPLER_GUARD

#include <Metal/Metal.h>
#include "Sampler.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Mtl_device;

//----------------------------------------------------------------------------------------------------------------------

class Mtl_sampler final : public Sampler {
public:
    Mtl_sampler(const Sampler_desc& desc, Mtl_device* device);

    Device* device() const override;

    inline auto sampler_state() const noexcept
    { return sampler_state_; }

private:
    void init_sampler_state_();

private:
    Mtl_device* device_;
    id<MTLSamplerState> sampler_state_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_MTL_SAMPLER_GUARD
