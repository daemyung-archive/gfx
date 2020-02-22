//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_LIMITATIONS_GUARD
#define GFX_LIMITATIONS_GUARD

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

constexpr auto max_vertex_input_attributes {16u};
constexpr auto max_vertex_input_bindings {8u};
constexpr auto max_shader_buffers {16u};
constexpr auto max_shader_textures {16u};
constexpr auto max_color_attachments {4u};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_LIMITATIONS_GUARD
