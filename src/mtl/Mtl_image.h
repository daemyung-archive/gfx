//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_MTL_IMAGE_GUARD
#define GFX_MTL_IMAGE_GUARD

#include <Metal/Metal.h>
#include "gfx/Image.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Mtl_device;

//----------------------------------------------------------------------------------------------------------------------

class Mtl_image final : public Image {
public:
    Mtl_image(const Image_desc& desc, Mtl_device* device);

    Device* device() const override;

    Image_type type() const override;

    Format format() const override;

    Extent extent() const override;

    uint8_t mip_levels() const override;

    uint8_t array_layers() const override;

    uint8_t samples() const override;

    Image_tiling tiling() const override;

    inline auto texture() const noexcept
    { return texture_; }

private:
    void init_texture_(const Image_desc& desc);

private:
    Mtl_device* device_;
    Image_type type_;
    Format format_;
    Extent extent_;
    id<MTLTexture> texture_;

    friend class Mtl_swap_chain;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_MTL_IMAGE_GUARD

