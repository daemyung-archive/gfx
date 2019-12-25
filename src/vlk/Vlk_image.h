//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_VLK_IMAGE_GUARD
#define GFX_VLK_IMAGE_GUARD

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include "gfx/Image.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Vlk_device;
class Vlk_swap_chain;

//----------------------------------------------------------------------------------------------------------------------

class Vlk_image final : public Image {
public:
    Vlk_image(const Image_desc& desc, Vlk_device* device);

    Vlk_image(const Image_desc& desc, Vlk_device* device, Vlk_swap_chain* swap_chain, VkImage image);

    ~Vlk_image() override;

    Device* device() const override;

    Image_type type() const override;

    Format format() const override;

    Extent extent() const override;

    uint8_t mip_levels() const override;

    uint8_t array_layers() const override;

    uint8_t samples() const override;

    inline auto& image() const noexcept
    { return image_; }

    inline auto& access_mask() const noexcept
    { return access_mask_; }

    inline auto& layout() const noexcept
    { return layout_; }

    inline auto& image_view() const noexcept
    { return image_view_; }

    inline auto& aspect_mask() const noexcept
    { return aspect_mask_; }

private:
    void init_image_and_alloc_(const Image_desc& desc);

    void init_image_view_(const Image_desc& desc);

    void fini_image_and_alloc_();

    void fini_image_view_();

private:
    Vlk_device* device_;
    Vlk_swap_chain* swap_chain_;
    Image_type type_;
    Format format_;
    Extent extent_;
    uint8_t mip_levels_;
    uint8_t array_layers_;
    uint8_t samplers_;
    VkImage image_;
    VmaAllocation alloc_;
    VkAccessFlags access_mask_;
    VkImageLayout layout_;
    VkImageView image_view_;
    VkImageAspectFlags aspect_mask_;

    friend class Vlk_swap_chain;
    friend class Vlk_render_encoder;
    friend class Vlk_blit_encoder;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_VLK_IMAGE_GUARD


