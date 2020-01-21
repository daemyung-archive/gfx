//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_FENCE_GUARD
#define GFX_FENCE_GUARD

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Device;

//----------------------------------------------------------------------------------------------------------------------

struct Fence_desc final {
    bool signaled {false};
};

//----------------------------------------------------------------------------------------------------------------------

class Fence {
public:
    virtual ~Fence() = default;

    virtual void wait_signal() = 0;

    virtual void reset() = 0;

    virtual Device* device() const = 0;

    virtual bool signaled() const = 0;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_FENCE_GUARD
