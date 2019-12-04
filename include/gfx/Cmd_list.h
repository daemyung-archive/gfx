//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_CMD_LIST_GUARD
#define GFX_CMD_LIST_GUARD

#include <cstdint>
#include <bitset>
#include <platform/Extent.h>
#include "gfx/enums.h"
#include "gfx/types.h"
#include "gfx/Clear_value.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Buffer;
class Image;
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

class Cmd_list {
public:
    virtual ~Cmd_list() = default;

    virtual void start() = 0;

    virtual void stop() = 0;

    virtual void reset() = 0;

    virtual void bind(Buffer* buffer, uint32_t index) = 0;

    virtual void bind(Buffer* buffer, Index_type type) = 0;

    virtual void bind(Buffer* buffer, const Pipeline_stages& stages, uint32_t index) = 0;

    virtual void bind(Pipeline* pipeline) = 0;

    virtual void begin(const Render_pass_state& state) = 0;

    virtual void end() = 0;

    virtual void set(const Viewport& viewport) = 0;

    virtual void set(const Scissor& scissor) = 0;

    virtual void draw(uint32_t count, uint32_t first = 0) = 0;

    virtual void draw_indexed(uint32_t count, uint32_t first = 0) = 0;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_CMD_LIST_GUARD
