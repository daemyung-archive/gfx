//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "std_lib.h"
#include "mtl_lib.h"
#include "Mtl_image.h"
#include "Mtl_device.h"

using namespace std;
using namespace Gfx_lib;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Mtl_image::Mtl_image(const Image_desc& desc, Mtl_device* device) :
    Image {desc},
    device_ {device},
    texture_ {nil}
{
    if (Image_type::two_dim == type_ || Image_type::cube == type_)
        init_texture_();
}

//----------------------------------------------------------------------------------------------------------------------

Device* Mtl_image::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_image::init_texture_()
{
    // configure a texture descriptor.
    auto descriptor = [MTLTextureDescriptor new];

    descriptor.textureType = to_MTLTextureType(type_);
    descriptor.pixelFormat = to_MTLPixelFormat(format_);
    descriptor.width = extent_.w;
    descriptor.height = extent_.h;
    descriptor.depth = extent_.d;
    descriptor.mipmapLevelCount = mip_levels_;
    descriptor.sampleCount = samples_;
    descriptor.arrayLength = array_layers_;
    descriptor.allowGPUOptimizedContents = YES;
    descriptor.resourceOptions = MTLResourceStorageModePrivate;
    descriptor.usage = MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget;

    // try to create a texture.
    texture_ = [device_->device() newTextureWithDescriptor:descriptor];

    if (!texture_)
        throw runtime_error("fail to create an image");
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
