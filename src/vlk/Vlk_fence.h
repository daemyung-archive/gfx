//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_VLK_FENCE_GUARD
#define GFX_VLK_FENCE_GUARD

#include <vulkan/vulkan.h>
#include "gfx/Fence.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Vlk_device;

//----------------------------------------------------------------------------------------------------------------------

class Vlk_fence final : public Fence {
public:
    Vlk_fence(const Fence_desc& desc, Vlk_device* device);

    void wait_signal() override;

    void reset() override;

    Device* device() const override;

    bool signaled() const override;

    inline auto fence() const noexcept
    { return fence_; }

private:
    void init_fence_(bool signaled);

private:
    Vlk_device* device_;
    VkFence fence_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_VLK_FENCE_GUARD
