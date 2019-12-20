//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "vlk_lib.h"
#include "Vlk_sampler.h"
#include "Vlk_device.h"

using namespace std;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Vlk_sampler::Vlk_sampler(const Sampler_desc& desc, Vlk_device* device) :
    Sampler(),
    device_ { device },
    min_ { desc.min },
    mag_ { desc.mag },
    mip_ { desc.mip },
    u_ { desc.u },
    v_ { desc.v },
    w_ { desc.w },
    sampler_ { VK_NULL_HANDLE }
{
    init_sampler_(desc);
}

//----------------------------------------------------------------------------------------------------------------------

Vlk_sampler::~Vlk_sampler()
{
    fini_sampler_();
}

//----------------------------------------------------------------------------------------------------------------------

Device* Vlk_sampler::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

Filter Vlk_sampler::min() const noexcept
{
    return min_;
}

//----------------------------------------------------------------------------------------------------------------------

Filter Vlk_sampler::mag() const noexcept
{
    return mag_;
}

//----------------------------------------------------------------------------------------------------------------------

Mip_filter Vlk_sampler::mip() const noexcept
{
    return mip_;
}

//----------------------------------------------------------------------------------------------------------------------

Address_mode Vlk_sampler::u() const noexcept
{
    return u_;
}

//----------------------------------------------------------------------------------------------------------------------

Address_mode Vlk_sampler::v() const noexcept
{
    return v_;
}

//----------------------------------------------------------------------------------------------------------------------

Address_mode Vlk_sampler::w() const noexcept
{
    return w_;
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_sampler::init_sampler_(const Sampler_desc& desc)
{
    // configure a sampler create info.
    VkSamplerCreateInfo create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    create_info.magFilter = convert<VkFilter>(desc.mag);
    create_info.minFilter = convert<VkFilter>(desc.min);
    create_info.mipmapMode = convert<VkSamplerMipmapMode>(desc.mip);
    create_info.addressModeU = convert<VkSamplerAddressMode>(desc.u);
    create_info.addressModeV = convert<VkSamplerAddressMode>(desc.v);
    create_info.addressModeW = convert<VkSamplerAddressMode>(desc.w);

    // try to create a sampler.
    if (vkCreateSampler(device_->device(), &create_info, nullptr, &sampler_))
        throw runtime_error("fail to create sampler");
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_sampler::fini_sampler_()
{
    vkDestroySampler(device_->device(), sampler_, nullptr);
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
