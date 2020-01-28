//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_OGL_CMD_BUFFER_GUARD
#define GFX_OGL_CMD_BUFFER_GUARD

#include <deque>
#include <functional>
#include <GLES3/gl3.h>
#include "Cmd_buffer.h"
#include "Ogl_pipeline.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Ogl_device;
class Ogl_buffer;
class Ogl_image;
class Ogl_sampler;
class Ogl_cmd_buffer;
class Ogl_framebuffer;

//----------------------------------------------------------------------------------------------------------------------

struct Ogl_vertex_stream final {
    Ogl_buffer* buffer {nullptr};
    uint64_t offset {0};
};

//----------------------------------------------------------------------------------------------------------------------

inline auto operator==(const Ogl_vertex_stream& lhs, const Ogl_vertex_stream& rhs)
{
    return (lhs.buffer == rhs.buffer) && (lhs.offset == rhs.offset);
}

//----------------------------------------------------------------------------------------------------------------------

struct Ogl_index_stream final {
    Ogl_buffer* buffer {nullptr};
    uint64_t offset {0};
    Index_type index_type {Index_type::invalid};
};

//----------------------------------------------------------------------------------------------------------------------

inline auto operator==(const Ogl_index_stream& lhs, const Ogl_index_stream& rhs)
{
    return (lhs.buffer == rhs.buffer) && (lhs.offset == rhs.offset) && (lhs.index_type == rhs.index_type);
}

//----------------------------------------------------------------------------------------------------------------------

template<typename T>
using Ogl_arg_array = std::array<T, 16>;

//----------------------------------------------------------------------------------------------------------------------

struct Ogl_arg_buffer {
    Ogl_buffer* buffer {nullptr};
    uint32_t offset {0};
};

//----------------------------------------------------------------------------------------------------------------------

struct Ogl_arg_texture {
    Ogl_image* image {nullptr};
    Ogl_sampler* sampler {nullptr};
};

//----------------------------------------------------------------------------------------------------------------------

class Ogl_arg_table final {
public:
    Ogl_arg_table();

    void clear();

    void arg_buffer(const Ogl_arg_buffer& arg_buffer, uint32_t index);

    void arg_texture(const Ogl_arg_texture& arg_texture, uint32_t index);

    inline auto arg_buffer(uint32_t index) const noexcept
    { return arg_buffers_[index]; }

    inline auto arg_texture(uint32_t index) const noexcept
    { return arg_textures_[index]; }

private:
    Ogl_arg_array<Ogl_arg_buffer> arg_buffers_;
    Ogl_arg_array<Ogl_arg_texture> arg_textures_;
};

//----------------------------------------------------------------------------------------------------------------------

class Ogl_render_encoder final : public Render_encoder {
public:
    Ogl_render_encoder(const Render_encoder_desc& desc, Ogl_device* device, Ogl_cmd_buffer* cmd_buffer);

    void end() override;

    void draw(uint32_t count, uint32_t first = 0) override;

    void draw_indexed(uint32_t count, uint32_t first = 0) override;

    void vertex_buffer(Buffer* buffer, uint64_t offset, uint32_t index) override;

    void index_buffer(Buffer* buffer, uint64_t offset, Index_type index_type) override;

    void shader_buffer(Pipeline_stage stage, Buffer* buffer, uint32_t offset, uint32_t index) override;

    void shader_texture(Pipeline_stage stage, Image* image, Sampler* sampler, uint32_t index) override;

    void pipeline(Pipeline* pipeline) override;

    void viewport(const Viewport& viewport) override;

    void scissor(const Scissor& scissor) override;

    Cmd_buffer* cmd_buffer() const override;

private:
    void init_framebuffer_(const Render_encoder_desc& desc);

    void begin_render_pass_(const Render_encoder_desc& desc);

    void end_render_pass_();

    void set_up_vertex_input_(const std::array<Ogl_vertex_stream, 2>& vertex_streams,
                              const Vertex_input& vertex_input);

    void set_up_rasterization_(const Rasterization& rasterization);

    void set_up_depth_stencil_(const Depth_stencil& depth_stencil);

    void set_up_color_blend_(const Color_blend& color_blend);


private:
    Ogl_device* device_;
    Ogl_cmd_buffer* cmd_buffer_;
    Ogl_framebuffer* framebuffer_;
    std::deque<std::function<void ()>> cmds_;
    std::array<Ogl_vertex_stream, 2> vertex_streams_;
    Ogl_index_stream index_stream_;
    Index_type index_type_;
    Ogl_arg_table arg_table_;
    Ogl_pipeline* pipeline_;
    Viewport viewport_;
    Scissor scissor_;
    std::vector<GLenum> discards_;
};

//----------------------------------------------------------------------------------------------------------------------

class Ogl_blit_encoder final : public Blit_encoder {
public:
    Ogl_blit_encoder(const Blit_encoder_desc& desc, Ogl_cmd_buffer* cmd_buffer);

    void copy(Buffer* src_buffer, Buffer* dst_buffer, const Buffer_copy_region& region) override;

    void copy(Buffer* src_buffer, Image* dst_image, const Buffer_image_copy_region& region) override;

    void copy(Image* src_buffer, Buffer* dst_image, const Buffer_image_copy_region& region) override;

    void end() override;

    Cmd_buffer* cmd_buffer() const override;

private:
    Ogl_cmd_buffer* cmd_buffer_;
    std::deque<std::function<void ()>> cmds_;
};

//----------------------------------------------------------------------------------------------------------------------

class Ogl_cmd_buffer final : public Cmd_buffer {
public:
    Ogl_cmd_buffer(Ogl_device* device);

    ~Ogl_cmd_buffer() override;

    std::unique_ptr<Render_encoder> create(const Render_encoder_desc& desc) override;

    std::unique_ptr<Blit_encoder> create(const Blit_encoder_desc& desc) override;

    void end() override;

    void reset() override;

    Device* device() const override;

private:
    Ogl_device* device_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_OGL_CMD_BUFFER_GUARD
