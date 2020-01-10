//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_MTL_IMAGE_GUARD
#define GFX_MTL_IMAGE_GUARD

#include <Metal/Metal.h>
#include "Image.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Mtl_device;

//----------------------------------------------------------------------------------------------------------------------

class Mtl_image final : public Image {
public:
    Mtl_image(const Image_desc& desc, Mtl_device* device);

    Device* device() const override;

    inline auto texture() const noexcept
    { return texture_; }

private:
    void init_texture_();

private:
    Mtl_device* device_;
    id<MTLTexture> texture_;

    friend class Mtl_swap_chain;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_MTL_IMAGE_GUARD

