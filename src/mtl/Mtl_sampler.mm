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
    Sampler {desc},
    device_ {device},
    sampler_state_ {nil}
{
    init_sampler_state_();
}

//----------------------------------------------------------------------------------------------------------------------

Device* Mtl_sampler::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_sampler::init_sampler_state_()
{
    // configure a sampler descriptor.
    auto descriptor = [MTLSamplerDescriptor new];

    descriptor.minFilter = to_MTLSamplerMinMagFilter(min_);
    descriptor.magFilter = to_MTLSamplerMinMagFilter(mag_);
    descriptor.mipFilter = to_MTLSamplerMipFilter(mip_);
    descriptor.sAddressMode = to_MTLSamplerAddressMode(u_);
    descriptor.tAddressMode = to_MTLSamplerAddressMode(v_);
    descriptor.rAddressMode = to_MTLSamplerAddressMode(w_);

    // try to create a sampler state.
    sampler_state_ = [device_->device() newSamplerStateWithDescriptor:descriptor];

    if (!sampler_state_)
        throw runtime_error("fail to create sampler");
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
