//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_IMAGE_GUARD
#define GFX_IMAGE_GUARD

#include <platform/Extent.h>
#include "enums.h"
#include "types.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Device;

//----------------------------------------------------------------------------------------------------------------------

struct Image_desc final {
    Image_type type {Image_type::two_dim};
    Format format {Format::invalid};
    Extent extent {0, 0, 1};
    uint8_t mip_levels {1};
    uint8_t array_layers {1};
    uint8_t samples {1};
};

//----------------------------------------------------------------------------------------------------------------------

class Image {
public:
    explicit Image(const Image_desc& desc) noexcept :
        type_ {desc.type},
        format_ {desc.format},
        extent_ {desc.extent},
        mip_levels_ {desc.mip_levels},
        array_layers_ {desc.array_layers},
        samples_ {desc.samples}
    {}

    virtual ~Image() = default;

    virtual Device* device() const = 0;

    inline Image_type type() const noexcept
    { return type_; }

    inline virtual Format format() const noexcept
    { return format_; }

    inline Extent extent() const noexcept
    { return extent_; }

    inline uint8_t mip_levels() const noexcept
    { return mip_levels_; }

    inline uint8_t array_layers() const noexcept
    { return array_layers_; }

    inline uint8_t samples() const noexcept
    { return samples_; }

protected:
    Image_type type_;
    Format format_;
    Extent extent_;
    uint8_t mip_levels_;
    uint8_t array_layers_;
    uint8_t samples_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_IMAGE_GUARD
