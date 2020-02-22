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
#include "limitations.h"
#include "enums.h"
#include "types.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Device;
class Buffer;
class Image;
class Sampler;
class Pipeline;
class Cmd_buffer;

//----------------------------------------------------------------------------------------------------------------------

struct Attachment final {
    Image* image {nullptr};
    Load_op load_op {Load_op::dont_care};
    Store_op store_op {Store_op::store};
    Clear_value clear_value;
};

//----------------------------------------------------------------------------------------------------------------------

struct Render_encoder_desc final {
    std::array<Attachment, max_color_attachments> colors;
    Attachment depth_stencil;
};

//----------------------------------------------------------------------------------------------------------------------

class Render_encoder {
public:
    virtual ~Render_encoder() = default;

    virtual void end() = 0;

    virtual void draw(uint32_t count, uint32_t first = 0) = 0;

    virtual void draw_indexed(uint32_t count, uint32_t first = 0) = 0;

    virtual void vertex_buffer(Buffer* buffer, uint64_t offset, uint32_t index) = 0;

    virtual void index_buffer(Buffer* buffer, uint64_t offset, Index_type index_type) = 0;

    virtual void shader_buffer(Buffer* buffer, uint32_t offset, uint32_t index) = 0;

    virtual void shader_texture(Image* image, Sampler* sampler, uint32_t index) = 0;

    virtual void pipeline(Pipeline* pipeline) = 0;

    virtual void viewport(const Viewport& viewport) = 0;

    virtual void scissor(const Scissor& scissor) = 0;

    virtual Cmd_buffer* cmd_buffer() const = 0;
};

//----------------------------------------------------------------------------------------------------------------------

struct Image_subresource final {
    uint32_t mip_level {0};
    uint32_t array_layer {0};
};

//----------------------------------------------------------------------------------------------------------------------

struct Buffer_copy_region final {
    uint64_t size {0};
    uint64_t src_offset {0};
    uint64_t dst_offset {0};
};

//----------------------------------------------------------------------------------------------------------------------

struct Buffer_image_copy_region final {
    uint32_t buffer_row_size {0};
    uint32_t buffer_image_height {0};
    uint32_t buffer_offset {0};
    Image_subresource image_subresource;
    Platform_lib::Extent image_extent {0, 0, 1};
    Offset image_offset {0, 0, 0};
};

//----------------------------------------------------------------------------------------------------------------------

struct Blit_encoder_desc final {
};

//----------------------------------------------------------------------------------------------------------------------

class Blit_encoder {
public:
    virtual ~Blit_encoder() = default;

    virtual void copy(Buffer* src_buffer, Buffer* dst_buffer, const Buffer_copy_region& region) = 0;

    virtual void copy(Buffer* src_buffer, Image* dst_image, const Buffer_image_copy_region& region) = 0;

    virtual void copy(Image* src_image, Buffer* dst_buffer, const Buffer_image_copy_region& region) = 0;

    virtual void end() = 0;

    virtual Cmd_buffer* cmd_buffer() const = 0;
};

//----------------------------------------------------------------------------------------------------------------------

struct Cmd_buffer_desc final {
};

//----------------------------------------------------------------------------------------------------------------------

class Cmd_buffer {
public:
    virtual ~Cmd_buffer() = default;

    virtual std::unique_ptr<Render_encoder> create(const Render_encoder_desc& desc) = 0;

    virtual std::unique_ptr<Blit_encoder> create(const Blit_encoder_desc& desc) = 0;

    virtual void end() = 0;

    virtual void reset() = 0;

    virtual Device* device() const = 0;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_CMD_BUFFER_GUARD
