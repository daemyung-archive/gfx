//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_VLK_DEVICE_GUARD
#define GFX_VLK_DEVICE_GUARD

#include <unordered_map>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <platform/Library.h>
#include "gfx/Device.h"
#include "Lru_cache.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Vlk_device;
class Vlk_cmd_buffer;
class Vlk_render_pass;
class Vlk_framebuffer;

//----------------------------------------------------------------------------------------------------------------------

class Vlk_device final : public Device {
public:
    Vlk_device();

    ~Vlk_device() override;

    std::unique_ptr<Buffer> make(const Buffer_desc& desc) override;

    std::unique_ptr<Image> make(const Image_desc& desc) override;

    std::unique_ptr<Sampler> make(const Sampler_desc& desc) override;

    std::unique_ptr<Shader> make(const Shader_desc& desc) override;

    std::unique_ptr<Pipeline> make(const Pipeline_desc<Pipeline_type::render>& desc) override;

    std::unique_ptr<Swap_chain> make(const Swap_chain_desc& desc) override;

    std::unique_ptr<Cmd_buffer> make_cmd_buffer() override;

    std::unique_ptr<Fence> make(const Fence_desc& desc) override;

    void submit(Cmd_buffer* cmd_buffer, Fence* fence = nullptr) override;

    void wait_idle() override;

    Vlk_render_pass* render_pass(const Render_pass_state& state);

    Vlk_render_pass* render_pass(const Output_merger_stage& state);

    Vlk_framebuffer* framebuffer(const Render_pass_state& state);

    inline auto instance() const noexcept
    { return instance_; }

    inline auto physical_device() const noexcept
    { return physical_device_; }

    inline auto queue_family_index() const noexcept
    { return queue_family_index_; }

    inline auto device() const noexcept
    { return device_; }

    inline auto queue() const noexcept
    { return queue_; }

    inline auto allocator() const noexcept
    { return allocator_; }

    inline auto command_pool() const noexcept
    { return command_pool_; }

private:
    void init_library_();

    void init_bootstrap_symbols_();

    void init_instance_();

    void init_instance_symbols_();

    void init_physical_device_();

    void init_queue_family_index_();

    void init_device_();

    void init_device_symbols_();

    void init_queue_();

    void init_allocator_();

    void init_command_pool_();

    void fini_instance_();

    void fini_device_();

    void fini_allocator_();

    void fini_command_pool_();

private:
    Platform_lib::Library library_;
    VkInstance instance_;
    VkPhysicalDevice physical_device_;
    uint32_t queue_family_index_;
    VkDevice device_;
    VkQueue queue_;
    VmaAllocator allocator_;
    VkCommandPool command_pool_;
    Lru_cache<Vlk_render_pass> render_pass_pool_;
    Lru_cache<Vlk_framebuffer> framebuffer_pool_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_VLK_DEVICE_GUARD
