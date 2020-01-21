//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_TYPES_GUARD
#define GFX_TYPES_GUARD

#include <platform/Extent.h>
#include <sc/enums.h>

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

using Extent = Platform_lib::Extent;

//----------------------------------------------------------------------------------------------------------------------

struct Offset final {
    uint64_t x {0};
    uint64_t y {0};
    uint64_t z {0};
};

//----------------------------------------------------------------------------------------------------------------------

struct Clear_value final {
    struct {
        float r;
        float g;
        float b;
        float a;
    };

    struct {
        float d;
        uint32_t s;
    };
};

//----------------------------------------------------------------------------------------------------------------------

struct Viewport final {
    float x {0.0f};
    float y {0.0f};
    float w {0.0f};
    float h {0.0f};
};

//----------------------------------------------------------------------------------------------------------------------

inline auto operator==(const Viewport& lhs, const Viewport& rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.w == rhs.w && lhs.h == rhs.h;
}

//----------------------------------------------------------------------------------------------------------------------

struct Scissor final {
    uint32_t x {0};
    uint32_t y {0};
    uint32_t w {0};
    uint32_t h {0};
};

//----------------------------------------------------------------------------------------------------------------------

inline auto operator==(const Scissor& lhs, const Scissor& rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.w == rhs.w && lhs.h == rhs.h;
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_TYPES_GUARD
