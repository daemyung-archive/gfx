//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "std_lib_modules.h"
#include "mtl_lib.h"
#include "Mtl_image.h"
#include "Mtl_device.h"

using namespace std;
using namespace Gfx_lib;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Mtl_image::Mtl_image(const Image_desc& desc, Mtl_device* device) :
    Image(),
    device_ { device },
    type_ { desc.type },
    format_ { desc.format },
    extent_ { desc.extent },
    texture_ { nil }
{
    if (Image_type::two_dim == desc.type || Image_type::cube == desc.type)
        init_texture_(desc);
}

//----------------------------------------------------------------------------------------------------------------------

Device* Mtl_image::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

Image_type Mtl_image::type() const
{
    return type_;
}

//----------------------------------------------------------------------------------------------------------------------

Format Mtl_image::format() const
{
    return format_;
}

//----------------------------------------------------------------------------------------------------------------------

Extent Mtl_image::extent() const
{
    return extent_;
}

//----------------------------------------------------------------------------------------------------------------------

uint8_t Mtl_image::mip_levels() const
{
    return [texture_ mipmapLevelCount];
}

//----------------------------------------------------------------------------------------------------------------------

uint8_t Mtl_image::array_layers() const
{
    return [texture_ arrayLength];
}

//----------------------------------------------------------------------------------------------------------------------

uint8_t Mtl_image::samples() const
{
    return [texture_ sampleCount];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_image::init_texture_(const Image_desc& desc)
{
    // configure a texture descriptor.
    auto descriptor = [MTLTextureDescriptor new];

    descriptor.textureType = convert<MTLTextureType>(desc.type);
    descriptor.pixelFormat = convert<MTLPixelFormat>(desc.format);
    descriptor.width = desc.extent.w;
    descriptor.height = desc.extent.h;
    descriptor.depth = desc.extent.d;
    descriptor.mipmapLevelCount = desc.mip_levels;
    descriptor.sampleCount = desc.samples;
    descriptor.arrayLength = desc.array_layers;
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
