//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_MTL_CMD_BUFFER_GUARD
#define GFX_MTL_CMD_BUFFER_GUARD

#include <array>
#include <unordered_map>
#include <Metal/Metal.h>
#include "Cmd_buffer.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Mtl_device;
class Mtl_buffer;
class Mtl_image;
class Mtl_sampler;
class Mtl_pipeline;
class Mtl_cmd_buffer;

//----------------------------------------------------------------------------------------------------------------------

struct Mtl_vertex_stream final {
    Mtl_buffer* buffer {nullptr};
    uint64_t offset {0};
};

//----------------------------------------------------------------------------------------------------------------------

inline auto operator==(const Mtl_vertex_stream& lhs, const Mtl_vertex_stream& rhs)
{
    return (lhs.buffer == rhs.buffer) && (lhs.offset == rhs.offset);
}

//----------------------------------------------------------------------------------------------------------------------

struct Mtl_index_stream final {
    Mtl_buffer* buffer {nullptr};
    uint64_t offset {0};
    Index_type index_type {Index_type::invalid};
};

//----------------------------------------------------------------------------------------------------------------------

inline auto operator==(const Mtl_index_stream& lhs, const Mtl_index_stream& rhs)
{
    return (lhs.buffer == rhs.buffer) && (lhs.offset == rhs.offset) && (lhs.index_type == rhs.index_type);
}

//----------------------------------------------------------------------------------------------------------------------

template<typename T>
using Mtl_arg_array = std::array<T, 16>;

//----------------------------------------------------------------------------------------------------------------------

struct Mtl_arg_buffer {
    Mtl_buffer* buffer {nullptr};
    uint32_t offset {0};
};

//----------------------------------------------------------------------------------------------------------------------

struct Mtl_arg_texture {
    Mtl_image* image {nullptr};
    Mtl_sampler* sampler {nullptr};
};

//----------------------------------------------------------------------------------------------------------------------

class Mtl_arg_table final {
public:
    Mtl_arg_table();

    void clear();

    void arg_buffer(const Mtl_arg_buffer& arg_buffer, uint32_t index);

    void arg_texture(const Mtl_arg_texture& arg_texture, uint32_t index);

    inline auto arg_buffer(uint32_t index) const noexcept
    { return arg_buffers_[index]; }

    inline auto arg_texture(uint32_t index) const noexcept
    { return arg_textures_[index]; }

private:
    Mtl_arg_array<Mtl_arg_buffer> arg_buffers_;
    Mtl_arg_array<Mtl_arg_texture> arg_textures_;
};

//----------------------------------------------------------------------------------------------------------------------

class Mtl_render_encoder final : public Render_encoder {
public:
    Mtl_render_encoder(const Render_encoder_desc& desc, Mtl_cmd_buffer* cmd_buffer);

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

    inline auto render_command_encoder() const noexcept
    { return render_command_encoder_; }

private:
    void init_render_command_encoder_(const Render_encoder_desc& desc);

private:
    Mtl_cmd_buffer* cmd_buffer_;
    id<MTLRenderCommandEncoder> render_command_encoder_;
    std::array<Mtl_vertex_stream, 2> vertex_streams_;
    Mtl_index_stream index_stream_;
    std::unordered_map<Pipeline_stage, Mtl_arg_table> arg_tables_;
    Mtl_pipeline* pipeline_;
};

//----------------------------------------------------------------------------------------------------------------------

class Mtl_blit_encoder final : public Blit_encoder {
public:
    Mtl_blit_encoder(const Blit_encoder_desc& desc, Mtl_cmd_buffer* cmd_buffer);

    void copy(Buffer* src_buffer, Buffer* dst_buffer, const Buffer_copy_region& region) override;

    void copy(Buffer* src_buffer, Image* dst_image, const Buffer_image_copy_region& region) override;

    void copy(Image* src_buffer, Buffer* dst_image, const Buffer_image_copy_region& region) override;

    void end() override;

    Cmd_buffer* cmd_buffer() const override;

    inline auto blit_command_encoder() const noexcept
    { return blit_command_encoder_; }

private:
    void init_blit_command_encoder_();

private:
    Mtl_cmd_buffer* cmd_buffer_;
    id<MTLBlitCommandEncoder> blit_command_encoder_;
};

//----------------------------------------------------------------------------------------------------------------------

class Mtl_cmd_buffer final : public Cmd_buffer {
public:
    Mtl_cmd_buffer(Mtl_device* device);

    std::unique_ptr<Render_encoder> create(const Render_encoder_desc& desc) override;

    std::unique_ptr<Blit_encoder> create(const Blit_encoder_desc& desc) override;

    void end() override;

    void reset() override;

    Device* device() const override;

    inline auto command_buffer() const noexcept
    { return command_buffer_; }

private:
    void init_command_buffer_();

private:
    Mtl_device* device_;
    id<MTLCommandBuffer> command_buffer_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_MTL_CMD_BUFFER_GUARD
