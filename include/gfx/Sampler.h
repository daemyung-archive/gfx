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

struct Sampler_desc final {
    Filter min {Filter::linear};
    Filter mag {Filter::linear};
    Mip_filter mip {Mip_filter::linear};
    Address_mode u {Address_mode::clamp_to_edge};
    Address_mode v {Address_mode::clamp_to_edge};
    Address_mode w {Address_mode::clamp_to_edge};
};

//----------------------------------------------------------------------------------------------------------------------

class Sampler {
public:
    explicit Sampler(const Sampler_desc& desc) noexcept :
        min_ {desc.min},
        mag_ {desc.mag},
        mip_ {desc.mip},
        u_ {desc.u},
        v_ {desc.v},
        w_ {desc.w}
    {}

    virtual ~Sampler() = default;

    virtual Device* device() const = 0;

    inline Filter min() const noexcept
    { return min_; }

    inline Filter mag() const noexcept
    { return mag_; }

    inline Mip_filter mip() const noexcept
    { return mip_; }

    inline Address_mode u() const noexcept
    { return u_; }

    inline Address_mode v() const noexcept
    { return v_; }

    inline Address_mode w() const noexcept
    { return w_; }

protected:
    Filter min_;
    Filter mag_;
    Mip_filter mip_;
    Address_mode u_;
    Address_mode v_;
    Address_mode w_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_SAMPLER_GUARD
