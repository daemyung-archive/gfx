//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "mtl_lib.h"
#include "Mtl_sampler.h"
#include "Mtl_device.h"

using namespace std;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Mtl_sampler::Mtl_sampler(const Sampler_desc& desc, Mtl_device* device) :
    Sampler(),
    device_ { device },
    min_ { desc.min },
    mag_ { desc.mag },
    mip_ { desc.mip },
    u_ { desc.u },
    v_ { desc.v },
    w_ { desc.w },
    sampler_state_ { nil }
{
    init_sampler_state_(desc);
}

//----------------------------------------------------------------------------------------------------------------------

Device* Mtl_sampler::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

Filter Mtl_sampler::min() const noexcept
{
    return min_;
}

//----------------------------------------------------------------------------------------------------------------------

Filter Mtl_sampler::mag() const noexcept
{
    return mag_;
}

//----------------------------------------------------------------------------------------------------------------------

Mip_filter Mtl_sampler::mip() const noexcept
{
    return mip_;
}

//----------------------------------------------------------------------------------------------------------------------

Address_mode Mtl_sampler::u() const noexcept
{
    return u_;
}

//----------------------------------------------------------------------------------------------------------------------

Address_mode Mtl_sampler::v() const noexcept
{
    return v_;
}

//----------------------------------------------------------------------------------------------------------------------

Address_mode Mtl_sampler::w() const noexcept
{
    return w_;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_sampler::init_sampler_state_(const Sampler_desc& desc)
{
    // configure a sampler descriptor.
    auto descriptor = [MTLSamplerDescriptor new];

    descriptor.minFilter = convert<MTLSamplerMinMagFilter>(desc.min);
    descriptor.magFilter = convert<MTLSamplerMinMagFilter>(desc.mag);
    descriptor.mipFilter = convert<MTLSamplerMipFilter>(desc.mip);
    descriptor.sAddressMode = convert<MTLSamplerAddressMode>(desc.u);
    descriptor.tAddressMode = convert<MTLSamplerAddressMode>(desc.v);
    descriptor.rAddressMode = convert<MTLSamplerAddressMode>(desc.w);

    // try to create a sampler state.
    sampler_state_ = [device_->device() newSamplerStateWithDescriptor:descriptor];

    if (!sampler_state_)
        throw runtime_error("fail to create sampler");

}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
