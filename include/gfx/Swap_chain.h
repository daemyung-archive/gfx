//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_SWAP_CHAIN_GUARD
#define GFX_SWAP_CHAIN_GUARD

#include <platform/Extent.h>
#include "gfx/enums.h"
#include "gfx/types.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Device;
class Image;

//----------------------------------------------------------------------------------------------------------------------

struct Swap_chain_desc {
    uint32_t image_count { 3 };
    Format image_format { Format::invalid };
    Extent image_extent { 0, 0, 1 };
    Color_space color_space { Color_space::srgb_non_linear };
    Present_mode present_mode { Present_mode::fifo };
    void* window { nullptr };
};

//----------------------------------------------------------------------------------------------------------------------

class Swap_chain {
public:
    virtual ~Swap_chain() = default;

    virtual Image* acquire() = 0;

    virtual void present() = 0;

    virtual Device* device() const = 0;

    virtual Format image_format() const = 0;

    virtual Extent image_extent() const = 0;

    virtual Color_space color_space() const = 0;

    virtual Present_mode present_mode() const = 0;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_SWAP_CHAIN_GUARD
