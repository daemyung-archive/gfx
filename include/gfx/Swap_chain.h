//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_SWAP_CHAIN_GUARD
#define GFX_SWAP_CHAIN_GUARD

#include <platform/Extent.h>
#include "enums.h"
#include "types.h"

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
    void* window { nullptr };
};

//----------------------------------------------------------------------------------------------------------------------

class Swap_chain {
public:
    explicit Swap_chain(const Swap_chain_desc& desc) noexcept :
        image_format_ {desc.image_format},
        image_extent_ {desc.image_extent},
        color_space_ {desc.color_space},
        frame_count_ {0}
    {}

    virtual ~Swap_chain() = default;

    virtual Image* acquire() = 0;

    virtual void present() = 0;

    virtual Device* device() const = 0;

    inline Format image_format() const noexcept
    { return image_format_; }

    inline Extent image_extent() const noexcept
    { return image_extent_; }

    inline Color_space color_space() const noexcept
    { return color_space_; }

    inline uint64_t frame_count() const noexcept
    { return frame_count_; }

protected:
    Format image_format_;
    Extent image_extent_;
    Color_space color_space_;
    uint64_t frame_count_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_SWAP_CHAIN_GUARD
