//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_VLK_CMD_BUFFER_GUARD
#define GFX_VLK_CMD_BUFFER_GUARD

#include <vulkan/vulkan.h>
#include "Cmd_buffer.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Vlk_device;
class Vlk_buffer;
class Vlk_image;
class Vlk_sampler;
class Vlk_pipeline;
class Vlk_swap_chain;

//----------------------------------------------------------------------------------------------------------------------

class Vlk_cmd_buffer final : public Cmd_buffer {
public:
    Vlk_cmd_buffer(Vlk_device* device);

    ~Vlk_cmd_buffer() override;

    void start() override;

    void stop() override;

    void reset() override;

    void bind(Buffer* buffer, uint32_t index) override;

    void bind(Buffer* buffer, Index_type type) override;

    void bind(Buffer* buffer, const Pipeline_stages& stages, uint32_t index) override;

    void bind(Image* image, const Pipeline_stages& stages, uint32_t index) override;

    void bind(Sampler* sampler, const Pipeline_stages& stages, uint32_t index) override;

    void bind(Pipeline* pipeline) override;

    void begin(const Render_pass_state& state) override;

    void end() override;

    void set(const Viewport& viewport) override;

    void set(const Scissor& scissor) override;

    void draw(uint32_t count, uint32_t first = 0) override;

    void draw_indexed(uint32_t count, uint32_t first = 0) override;

    void copy(Buffer* src_buffer, Buffer* dst_buffer, const Buffer_copy_region& region) override;

    void copy(Buffer* src_buffer, Image* dst_image, const Buffer_image_copy_region& region) override;

    void copy(Image* src_buffer, Buffer* dst_image, const Buffer_image_copy_region& region) override;

    Device* device() const override;

    inline auto& command_buffer() const noexcept
    { return command_buffer_; }

private:
    void init_command_buffer_();

    void fini_command_buffer_();

private:
    Vlk_device* device_;
    VkCommandBuffer command_buffer_;
    std::array<Vlk_buffer*, 2> vertex_buffers_;
    Vlk_buffer* index_buffer_;
    Vlk_pipeline* pipeline_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_VLK_CMD_BUFFER_GUARD
