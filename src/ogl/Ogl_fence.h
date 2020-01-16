//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_OGL_FENCE_GUARD
#define GFX_OGL_FENCE_GUARD

#include <GLES3/gl3.h>
#include "Fence.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Ogl_device;

//----------------------------------------------------------------------------------------------------------------------

class Ogl_fence final : public Fence {
public:
    Ogl_fence(const Fence_desc& desc, Ogl_device* device);

    void wait_signal() override;

    void reset() override;

    Device* device() const override;

    bool signaled() const override;

private:
    Ogl_device* device_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_OGL_FENCE_GUARD
