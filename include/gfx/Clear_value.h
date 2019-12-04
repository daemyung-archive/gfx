//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_CLEAR_VALUE_GUARD
#define GFX_CLEAR_VALUE_GUARD

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------

struct Clear_value {
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

}

#endif // GFX_CLEAR_VALUE_GUARD
