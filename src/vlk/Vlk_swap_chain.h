//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_VLK_SWAP_CHAIN_GUARD
#define GFX_VLK_SWAP_CHAIN_GUARD

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#include "gfx/Swap_chain.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Vlk_device;
class Vlk_image;
class Vlk_cmd_buffer;
class Vlk_fence;

//----------------------------------------------------------------------------------------------------------------------

class Vlk_swap_chain final : public Swap_chain {
public:
    Vlk_swap_chain(const Swap_chain_desc& desc, Vlk_device* device);

    ~Vlk_swap_chain() override;

    Image* acquire() override;

    void present() override;

    Device* device() const override;

    Format image_format() const override;

    Extent image_extent() const override;

    Color_space color_space() const override;

    Present_mode present_mode() const override;

    inline auto& swapchain() const noexcept
    { return swapchain_; }

    inline auto& image_index() const noexcept
    { return image_index_; }

private:
    void init_surface_(const Swap_chain_desc& desc);

    void init_swapchain_(const Swap_chain_desc& desc);

    void init_images_();

    void init_fences_();

    void init_cmd_buffers_();

    void init_semaphores_();

    void fini_surface_();

    void fini_swapchain_();

    void fini_semaphores_();

private:
    Vlk_device* device_;
    Format image_format_;
    Extent image_extent_;
    Color_space color_space_;
    Present_mode present_mode_;
    void* window_;
    VkSurfaceKHR surface_;
    VkSwapchainKHR swapchain_;
    std::vector<std::unique_ptr<Vlk_image>> images_;
    uint32_t image_index_;
    std::vector<std::unique_ptr<Vlk_fence>> fences_;
    std::vector<std::unique_ptr<Vlk_cmd_buffer>> cmd_buffers_;
    std::vector<VkSemaphore> acquire_semaphores_;
    std::vector<VkSemaphore> submit_semaphores_;
    uint32_t index_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_VLK_SWAP_CHAIN_GUARD
