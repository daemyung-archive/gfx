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

//----------------------------------------------------------------------------------------------------------------------

template<typename T>
struct Mtl_arg {
    T* res { nullptr };
};

//----------------------------------------------------------------------------------------------------------------------

template<>
struct Mtl_arg<Mtl_buffer> {
    Mtl_buffer* res { nullptr };
    uint32_t offset;
};

//----------------------------------------------------------------------------------------------------------------------

class Mtl_arg_table final {
public:
    Mtl_arg_table();

    void clear();

    bool set(Mtl_arg<Mtl_buffer>&& arg, uint32_t index);

    bool set(Mtl_arg<Mtl_image>&& arg, uint32_t index);

    bool set(Mtl_arg<Mtl_sampler>&& arg, uint32_t index);

    template<typename T>
    Mtl_arg<T> get(size_t) const noexcept;

    template<>
    Mtl_arg<Mtl_buffer> get(size_t index) const noexcept
    { return buffers_[index]; }

    template<>
    Mtl_arg<Mtl_image> get(size_t index) const noexcept
    { return images_[index]; }

    template<>
    Mtl_arg<Mtl_sampler> get(size_t index) const noexcept
    { return samplers_[index]; }

private:
    std::vector<Mtl_arg<Mtl_buffer>> buffers_;
    std::vector<Mtl_arg<Mtl_image>> images_;
    std::vector<Mtl_arg<Mtl_sampler>> samplers_;
};

//----------------------------------------------------------------------------------------------------------------------

class Mtl_cmd_buffer final : public Cmd_buffer {
public:
    Mtl_cmd_buffer(Mtl_device* device);

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

    inline auto command_buffer() const noexcept
    { return command_buffer_; }

private:
    void bind_buffer_(Mtl_buffer* buffer, uint32_t index);

    void bind_buffer_(Mtl_buffer* buffer, Pipeline_stage stage, uint32_t index, uint32_t offset = 0);

    void bind_image_(Mtl_image* image, Pipeline_stage stage, uint32_t index);

    void bind_sampler_(Mtl_sampler* sampler, Pipeline_stage stage, uint32_t index);

    void bind_pipeline_(Mtl_pipeline* pipeline);

private:
    Mtl_device* device_;
    id<MTLCommandBuffer> command_buffer_;
    id<MTLRenderCommandEncoder> render_encoder_;
    id<MTLComputeCommandEncoder> compute_encoder_;
    std::array<Mtl_buffer*, 2> vertex_buffers_;
    Mtl_buffer* index_buffer_;
    MTLIndexType index_type_;
    std::unordered_map<Pipeline_stage, Mtl_arg_table> arg_tables_;
    Mtl_pipeline* pipeline_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_MTL_CMD_BUFFER_GUARD
