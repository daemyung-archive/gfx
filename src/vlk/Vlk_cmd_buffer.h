//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_VLK_CMD_BUFFER_GUARD
#define GFX_VLK_CMD_BUFFER_GUARD

#include <map>
#include <unordered_map>
#include <deque>
#include <functional>
#include <vulkan/vulkan.h>
#include "Cmd_buffer.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Vlk_device;
class Vlk_buffer;
class Vlk_image;
class Vlk_sampler;
class Vlk_pipeline;
class Vlk_cmd_buffer;
class Vlk_render_pass;
class Vlk_framebuffer;

//----------------------------------------------------------------------------------------------------------------------

template<typename T>
class Vlk_arg_array : public std::array<T, 16> {
public:
    VkDescriptorSet desc_set {VK_NULL_HANDLE};
    uint32_t dirty_flags {0};
};

//----------------------------------------------------------------------------------------------------------------------

struct Vlk_arg {
    Vlk_buffer* buffer {nullptr};
    Vlk_image* image {nullptr};
    Vlk_sampler* sampler {nullptr};
    uint32_t offset {0};
    uint32_t dirty_flags {false};
};

//----------------------------------------------------------------------------------------------------------------------

class Vlk_arg_table final {
public:
    Vlk_arg_table();

    void clear();

    inline Vlk_arg_array<Vlk_arg>& operator[](size_t index) noexcept
    { return args_[index]; }

private:
    std::array<Vlk_arg_array<Vlk_arg>, 2> args_;
};

//----------------------------------------------------------------------------------------------------------------------

class Vlk_render_encoder final : public Render_encoder {
public:
    Vlk_render_encoder(const Render_encoder_desc& desc, Vlk_device* device, Vlk_cmd_buffer* cmd_buffer);

    void end() override;

    void draw(uint32_t count, uint32_t first = 0) override;

    void draw_indexed(uint32_t count, uint32_t first = 0) override;

    void vertex_buffer(Buffer* buffer, uint32_t index) override;

    void index_buffer(Buffer* buffer, Index_type index_type) override;

    void shader_buffer(Pipeline_stage stage, Buffer* buffer, uint32_t offset, uint32_t index) override;

    void shader_texture(Pipeline_stage stage, Image* image, Sampler* sampler, uint32_t index) override;

    void pipeline(Pipeline* pipeline) override;

    void viewport(const Viewport& viewport) override;

    void scissor(const Scissor& scissor) override;

    Cmd_buffer* cmd_buffer() const override;

private:
    void begin_render_pass_(const Render_encoder_desc& desc);

    void end_render_pass_();

    void update_desc_sets_();

    void bind_desc_sets_();

private:
    Vlk_device* device_;
    Vlk_cmd_buffer* cmd_buffer_;
    std::map<uint32_t, std::deque<std::function<void ()>>> cmds_;
    std::array<Vlk_buffer*, 2> vertex_buffers_;
    Vlk_buffer* index_buffer_;
    std::unordered_map<Pipeline_stage, Vlk_arg_table> arg_tables_;
    Vlk_pipeline* pipeline_;
    Vlk_render_pass* render_pass_;
    Vlk_framebuffer* framebuffer_;
    Viewport viewport_;
    Scissor scissor_;
};

//----------------------------------------------------------------------------------------------------------------------

class Vlk_blit_encoder final : public Blit_encoder {
public:
    Vlk_blit_encoder(const Blit_encoder_desc& desc, Vlk_cmd_buffer* cmd_buffer);

    void copy(Buffer* src_buffer, Buffer* dst_buffer, const Buffer_copy_region& region) override;

    void copy(Buffer* src_buffer, Image* dst_image, const Buffer_image_copy_region& region) override;

    void copy(Image* src_buffer, Buffer* dst_image, const Buffer_image_copy_region& region) override;

    void end() override;

    Cmd_buffer* cmd_buffer() const override;

private:
    Vlk_cmd_buffer* cmd_buffer_;
    std::deque<std::function<void ()>> cmds_;
};

//----------------------------------------------------------------------------------------------------------------------

class Vlk_cmd_buffer final : public Cmd_buffer {
public:
    Vlk_cmd_buffer(Vlk_device* device);

    ~Vlk_cmd_buffer() override;

    std::unique_ptr<Render_encoder> create(const Render_encoder_desc& desc) override;

    std::unique_ptr<Blit_encoder> create(const Blit_encoder_desc& desc) override;

    void end() override;

    void reset() override;

    Device* device() const override;

    inline auto& command_buffer() const noexcept
    { return command_buffer_; }

private:
    void init_command_buffer_();

    void fini_command_buffer_();

    void begin_command_buffer_();

private:
    Vlk_device* device_;
    VkCommandBuffer command_buffer_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_VLK_CMD_BUFFER_GUARD
