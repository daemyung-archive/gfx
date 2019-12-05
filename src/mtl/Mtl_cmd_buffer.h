//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_MTL_CMD_BUFFER_GUARD
#define GFX_MTL_CMD_BUFFER_GUARD

#include <array>
#include <unordered_map>
#include <Metal/Metal.h>
#include "gfx/Cmd_buffer.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Mtl_device;
class Mtl_buffer;
class Mtl_image;
class Mtl_sampler;
class Mtl_pipeline;

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

    void bind_render_pipeline_(Mtl_pipeline* pipeline);

    void bind_compute_pipeline_(Mtl_pipeline* pipeline);

private:
    Mtl_device* device_;
    id<MTLCommandBuffer> command_buffer_;
    id<MTLRenderCommandEncoder> render_encoder_;
    id<MTLComputeCommandEncoder> compute_encoder_;
    std::array<Mtl_buffer*, 2> binding_vertex_buffers_;
    Mtl_buffer* binding_index_buffer_;
    MTLIndexType binding_index_type_;
    std::unordered_map<Pipeline_stage, std::array<Mtl_buffer*, 8>> binding_uniform_buffers_;
    std::unordered_map<Pipeline_stage, std::array<uint32_t, 8>> binding_uniform_offsets_;
    std::unordered_map<Pipeline_stage, std::array<Mtl_image*, 8>> binding_images_;
    std::unordered_map<Pipeline_stage, std::array<Mtl_sampler*, 8>> binding_samplers_;
    Mtl_pipeline* binding_pipeline_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_MTL_CMD_BUFFER_GUARD
