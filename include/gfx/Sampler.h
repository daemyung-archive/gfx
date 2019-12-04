//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_SAMPLER_GUARD
#define GFX_SAMPLER_GUARD

#include "gfx/enums.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

struct Sampler_desc {
    Filter min;
    Filter mag;
    Mip_filter mip;
    Address_mode u;
    Address_mode v;
    Address_mode w;
};

//----------------------------------------------------------------------------------------------------------------------

class Sampler {
public:
    virtual ~Sampler() = default;

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
