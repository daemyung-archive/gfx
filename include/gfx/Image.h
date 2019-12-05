//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_IMAGE_GUARD
#define GFX_IMAGE_GUARD

#include <platform/Extent.h>
#include "gfx/enums.h"
#include "gfx/types.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Device;

//----------------------------------------------------------------------------------------------------------------------

struct Image_desc {
    Image_type type { Image_type::two_dim };
    Format format { Format::invalid };
    Extent extent { 0, 0, 1 };
    uint8_t mip_levels { 1 };
    uint8_t array_layers { 1 };
    uint8_t samples { 1 };
    Image_tiling image_tiling { Image_tiling::optimal };
};

//----------------------------------------------------------------------------------------------------------------------

class Image {
public:
    virtual ~Image() = default;

    virtual Device* device() const = 0;

    virtual Image_type type() const = 0;

    virtual Format format() const = 0;

    virtual Extent extent() const = 0;

    virtual uint8_t mip_levels() const = 0;

    virtual uint8_t array_layers() const = 0;

    virtual uint8_t samples() const = 0;

    virtual Image_tiling tiling() const = 0;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_IMAGE_GUARD
