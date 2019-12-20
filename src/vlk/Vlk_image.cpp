//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "std_lib.h"
#include "vlk_lib.h"
#include "Vlk_image.h"
#include "Vlk_device.h"
#include "Vlk_swap_chain.h"

using namespace std;
using namespace Gfx_lib;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Vlk_image::Vlk_image(const Image_desc& desc, Vlk_device* device) :
    Image(),
    device_ { device },
    swap_chain_ { nullptr },
    type_ { desc.type },
    format_ { desc.format },
    extent_ { desc.extent },
    mip_levels_ { desc.mip_levels },
    array_layers_ { desc.array_layers },
    samplers_ { desc.samples },
    image_ { VK_NULL_HANDLE },
    alloc_ { VK_NULL_HANDLE },
    access_mask_ { 0 },
    layout_ { VK_IMAGE_LAYOUT_UNDEFINED },
    image_view_ { VK_NULL_HANDLE },
    aspect_mask_ { convert<VkImageAspectFlags>(desc.format) }
{
    init_image_and_alloc_(desc);
    init_image_view_(desc);
}


//----------------------------------------------------------------------------------------------------------------------

Vlk_image::Vlk_image(const Image_desc& desc, Vlk_device* device, Vlk_swap_chain* swap_chain, VkImage image) :
    Image(),
    device_ { device },
    swap_chain_ { swap_chain },
    type_ { desc.type },
    format_ { desc.format },
    extent_ { desc.extent },
    mip_levels_ { desc.mip_levels },
    array_layers_ { desc.array_layers },
    samplers_ { desc.samples },
    image_ { image },
    alloc_ { VK_NULL_HANDLE },
    layout_ { VK_IMAGE_LAYOUT_UNDEFINED },
    image_view_ { VK_NULL_HANDLE },
    aspect_mask_ { convert<VkImageAspectFlags>(desc.format) }
{
    init_image_view_(desc);
}

//----------------------------------------------------------------------------------------------------------------------

Vlk_image::~Vlk_image()
{
    fini_image_view_();

    if (Image_type::two_dim == type_ || Image_type::cube == type_)
        fini_image_and_alloc_();
}

//----------------------------------------------------------------------------------------------------------------------

Device* Vlk_image::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

Image_type Vlk_image::type() const
{
    return type_;
}

//----------------------------------------------------------------------------------------------------------------------

Format Vlk_image::format() const
{
    return format_;
}

//----------------------------------------------------------------------------------------------------------------------

Extent Vlk_image::extent() const
{
    return extent_;
}

//----------------------------------------------------------------------------------------------------------------------

uint8_t Vlk_image::mip_levels() const
{
    return mip_levels_;
}

//----------------------------------------------------------------------------------------------------------------------

uint8_t Vlk_image::array_layers() const
{
    return array_layers_;
}

//----------------------------------------------------------------------------------------------------------------------

uint8_t Vlk_image::samples() const
{
    return samplers_;
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_image::init_image_and_alloc_(const Image_desc& desc)
{
    // configure the required image usage.
    constexpr auto usage {
        VK_IMAGE_USAGE_SAMPLED_BIT |
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
        VK_IMAGE_USAGE_TRANSFER_DST_BIT
    };

    // configure an image create info.
    VkImageCreateInfo create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    create_info.imageType = convert<VkImageType>(desc.type);
    create_info.format = convert<VkFormat>(desc.format);
    create_info.extent = ::convert<VkExtent3D>(desc.extent);
    create_info.mipLevels = desc.mip_levels;
    create_info.arrayLayers = desc.array_layers;
    create_info.samples = static_cast<VkSampleCountFlagBits>(desc.samples);
    create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    create_info.usage = usage;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    // configure an allocation create info.
    VmaAllocationCreateInfo alloc_create_info {};

    alloc_create_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    // try to create an image and an allocation.
    if (vmaCreateImage(device_->allocator(), &create_info, &alloc_create_info, &image_, &alloc_, nullptr))
        throw runtime_error("fail to create an image");
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_image::init_image_view_(const Image_desc& desc)
{
    // configure an image view create info.
    VkImageViewCreateInfo create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = image_;
    create_info.viewType = convert<VkImageViewType>(desc.type);
    create_info.format = convert<VkFormat>(desc.format);
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange.aspectMask = convert<VkImageAspectFlags>(desc.format);
    create_info.subresourceRange.levelCount = desc.mip_levels;
    create_info.subresourceRange.layerCount = desc.array_layers;

    // try to create an image view.
    if (vkCreateImageView(device_->device(), &create_info, nullptr, &image_view_))
        throw runtime_error("fail to create an image");
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_image::fini_image_and_alloc_()
{
    vkDestroyImage(device_->device(), image_, nullptr);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_image::fini_image_view_()
{
    vkDestroyImageView(device_->device(), image_view_, nullptr);
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
