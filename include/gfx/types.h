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
using Stage = Sc_lib::Stage;

//----------------------------------------------------------------------------------------------------------------------

struct Viewport {
    float x;
    float y;
    float w;
    float h;
};

//----------------------------------------------------------------------------------------------------------------------

struct Scissor {
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_TYPES_GUARD
