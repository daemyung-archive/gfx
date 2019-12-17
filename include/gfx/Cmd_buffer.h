//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_CMD_BUFFER_GUARD
#define GFX_CMD_BUFFER_GUARD

#include <cstdint>
#include <array>
#include <bitset>
#include <platform/Extent.h>
#include "enums.h"
#include "types.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Device;
class Buffer;
class Image;
class Sampler;
class Pipeline;

using Extent = Platform_lib::Extent;
using Pipeline_stages = std::bitset<8>;

//----------------------------------------------------------------------------------------------------------------------

struct Render_pass_attachment_state {
    Image* image { nullptr };
    Load_op load_op { Load_op::dont_care };
    Store_op store_op { Store_op::store };
    Clear_value clear_value;
};

//----------------------------------------------------------------------------------------------------------------------

struct Render_pass_state {
    std::array<Render_pass_attachment_state, 4> colors;
    Render_pass_attachment_state depth_stencil;
};

//----------------------------------------------------------------------------------------------------------------------

struct Image_subresource {
    uint32_t mip_level { 0 };
    uint32_t array_layer { 0 };
};

//----------------------------------------------------------------------------------------------------------------------

struct Buffer_copy_region {
    uint64_t size { 0 };
    uint64_t src_offset { 0 };
    uint64_t dst_offset { 0 };
};

//----------------------------------------------------------------------------------------------------------------------

struct Buffer_image_copy_region {
    uint32_t buffer_row_size { 0 };
    uint32_t buffer_image_height { 0 };
    uint32_t buffer_offset { 0 };
    Image_subresource image_subresource;
    Extent image_extent { 0, 0, 1 };
    Offset image_offset { 0, 0, 0 };
};

//----------------------------------------------------------------------------------------------------------------------

class Cmd_buffer {
public:
    virtual ~Cmd_buffer() = default;

    virtual void start() = 0;

    virtual void stop() = 0;

    virtual void reset() = 0;

    virtual void bind(Buffer* buffer, uint32_t index) = 0;

    virtual void bind(Buffer* buffer, Index_type type) = 0;

    virtual void bind(Buffer* buffer, const Pipeline_stages& stages, uint32_t index) = 0;

    virtual void bind(Image* image, const Pipeline_stages& stages, uint32_t index) = 0;

    virtual void bind(Sampler* sampler, const Pipeline_stages& stages, uint32_t index) = 0;

    virtual void bind(Pipeline* pipeline) = 0;

    virtual void begin(const Render_pass_state& state) = 0;

    virtual void end() = 0;

    virtual void set(const Viewport& viewport) = 0;

    virtual void set(const Scissor& scissor) = 0;

    virtual void draw(uint32_t count, uint32_t first = 0) = 0;

    virtual void draw_indexed(uint32_t count, uint32_t first = 0) = 0;

    virtual void copy(Buffer* src_buffer, Buffer* dst_buffer, const Buffer_copy_region& region) = 0;

    virtual void copy(Buffer* src_buffer, Image* dst_image, const Buffer_image_copy_region& region) = 0;

    virtual void copy(Image* src_image, Buffer* dst_buffer, const Buffer_image_copy_region& region) = 0;

    virtual Device* device() const = 0;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_CMD_BUFFER_GUARD
