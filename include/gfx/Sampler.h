//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_SAMPLER_GUARD
#define GFX_SAMPLER_GUARD

#include "enums.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Device;

//----------------------------------------------------------------------------------------------------------------------

struct Sampler_desc {
    Filter min { Filter::linear };
    Filter mag { Filter::linear };
    Mip_filter mip { Mip_filter::linear };
    Address_mode u { Address_mode::clamp_to_edge };
    Address_mode v { Address_mode::clamp_to_edge };
    Address_mode w { Address_mode::clamp_to_edge };
};

//----------------------------------------------------------------------------------------------------------------------

class Sampler {
public:
    virtual ~Sampler() = default;

    virtual Device* device() const = 0;

    virtual Filter min() const noexcept = 0;

    virtual Filter mag() const noexcept = 0;

    virtual Mip_filter mip() const noexcept = 0;

    virtual Address_mode u() const noexcept = 0;

    virtual Address_mode v() const noexcept = 0;

    virtual Address_mode w() const noexcept = 0;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_SAMPLER_GUARD
