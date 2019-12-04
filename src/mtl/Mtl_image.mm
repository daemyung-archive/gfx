//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "mtl/Mtl_image.h"
#include "mtl/Mtl_device.h"
#include "mtl/mtl_lib_modules.h"
#include "std_lib_modules.h"

using namespace std;
using namespace Gfx_lib;

namespace {

//----------------------------------------------------------------------------------------------------------------------

inline auto make(const Image_desc& desc)
{
    auto descriptor = [MTLTextureDescriptor new];

    descriptor.textureType = convert(desc.type);
    descriptor.pixelFormat = convert<MTLPixelFormat>(desc.format);
    descriptor.width = desc.extent.w;
    descriptor.height = desc.extent.h;
    descriptor.depth = desc.extent.d;
    descriptor.mipmapLevelCount = desc.mip_levels;
    descriptor.sampleCount = desc.samples;
    descriptor.arrayLength = desc.array_layers;
    descriptor.allowGPUOptimizedContents = (Image_tiling::optimal == desc.image_tiling);

    return descriptor;
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace

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

Image_tiling Mtl_image::tiling() const
{
    return [texture_ allowGPUOptimizedContents] ? Image_tiling::optimal : Image_tiling::linear;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_image::init_texture_(const Image_desc& desc)
{
    auto descriptor = make(desc);

    descriptor.resourceOptions = MTLResourceStorageModePrivate;
    descriptor.usage = MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget;

    texture_ = [device_->device() newTextureWithDescriptor:descriptor];

    if (!texture_)
        throw runtime_error("fail to create an image");
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
