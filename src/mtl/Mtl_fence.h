//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_MTL_FENCE_GUARD
#define GFX_MTL_FENCE_GUARD

#include <dispatch/dispatch.h>
#include "Fence.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Mtl_device;

//----------------------------------------------------------------------------------------------------------------------

class Mtl_fence final : public Fence {
public:
    Mtl_fence(const Fence_desc& desc, Mtl_device* device);

    void wait_signal() override;

    void reset() override;

    Device* device() const override;

    bool signaled() const override;

    inline auto semaphore() const noexcept
    { return semaphore_; }

private:
    void init_semaphore_();

private:
    Mtl_device* device_;
    bool signaled_;
    dispatch_semaphore_t semaphore_;

    friend class Mtl_device;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_MTL_FENCE_GUARD
