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

namespace {

//----------------------------------------------------------------------------------------------------------------------

inline bool is_color_format(Format format)
{
    switch (format) {
        case Format::rgba8_unorm:
        case Format::bgra8_unorm:
            return true;
        default:
            return false;
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline auto is_depth_stencil_format(Format format)
{
    switch (format) {
        case Format::d24_unorm_s8_uint:
            return true;
        default:
            return false;
    }
}

//----------------------------------------------------------------------------------------------------------------------

}

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Vlk_image::Vlk_image(const Image_desc& desc, Vlk_device* device) :
    Image {desc},
    device_ {device },
    swap_chain_ {nullptr},
    image_ { VK_NULL_HANDLE },
    alloc_ { VK_NULL_HANDLE },
    access_mask_ { 0 },
    layout_ { VK_IMAGE_LAYOUT_UNDEFINED },
    image_view_ { VK_NULL_HANDLE },
    aspect_mask_ { to_VkImageAspectFlags(format_) }
{
    init_image_and_alloc_();
    init_image_view_();
}


//----------------------------------------------------------------------------------------------------------------------

Vlk_image::Vlk_image(const Image_desc& desc, Vlk_device* device, Vlk_swap_chain* swap_chain, VkImage image) :
    Image {desc},
    device_ {device},
    swap_chain_ {swap_chain},
    image_ {image},
    alloc_ {VK_NULL_HANDLE},
    layout_ {VK_IMAGE_LAYOUT_UNDEFINED},
    image_view_ {VK_NULL_HANDLE},
    aspect_mask_ { to_VkImageAspectFlags(format_) }
{
    init_image_view_();
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

void Vlk_image::init_image_and_alloc_()
{
    // configure the required image usage.
    auto usage {VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT};

    if (is_color_format(format_)) {
        usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
        usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }

    if (is_depth_stencil_format(format_))
        usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    // configure an image create info.
    VkImageCreateInfo create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    create_info.imageType = to_VkImageType(type_);
    create_info.format = to_VkFormat(format_);
    create_info.extent = to_VkExtent3D(extent_);
    create_info.mipLevels = mip_levels_;
    create_info.arrayLayers = array_layers_;
    create_info.samples = static_cast<VkSampleCountFlagBits>(samples_);
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

void Vlk_image::init_image_view_()
{
    // configure an image view create info.
    VkImageViewCreateInfo create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = image_;
    create_info.viewType = to_VkImageViewType(type_);
    create_info.format = to_VkFormat(format_);
    create_info.subresourceRange.aspectMask = aspect_mask_;
    create_info.subresourceRange.levelCount = mip_levels_;
    create_info.subresourceRange.layerCount = array_layers_;

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
