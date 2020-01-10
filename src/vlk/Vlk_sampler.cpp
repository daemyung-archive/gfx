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
    Sampler {desc},
    device_ {device},
    sampler_ {VK_NULL_HANDLE}
{
    init_sampler_();
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

void Vlk_sampler::init_sampler_()
{
    // configure a sampler create info.
    VkSamplerCreateInfo create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    create_info.magFilter = to_VkFilter(mag_);
    create_info.minFilter = to_VkFilter(min_);
    create_info.mipmapMode = to_VkSamplerMipmapMode(mip_);
    create_info.addressModeU = to_VkSamplerAddressMode(u_);
    create_info.addressModeV = to_VkSamplerAddressMode(v_);
    create_info.addressModeW = to_VkSamplerAddressMode(w_);

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
