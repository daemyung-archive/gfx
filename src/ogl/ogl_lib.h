//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_OGL_LIB_GUARD
#define GFX_OGL_LIB_GUARD

#include <stdexcept>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <sc/enums.h>
#include "enums.h"

#define APPLY_OGL_DRAW_BUFFERS_INDEXED_SYMBOLS(macro) \
    macro(glEnablei) \
    macro(glDisablei) \
    macro(glBlendEquationi) \
    macro(glBlendEquationSeparatei) \
    macro(glBlendFunci) \
    macro(glBlendFuncSeparatei) \
    macro(glColorMaski) \
    macro(glIsEnabledi)

using PFN_glEnablei = PFNGLENABLEIOESPROC;
using PFN_glDisablei = PFNGLDISABLEIOESPROC;
using PFN_glBlendEquationi = PFNGLBLENDEQUATIONIOESPROC;
using PFN_glBlendEquationSeparatei = PFNGLBLENDEQUATIONSEPARATEIOESPROC;
using PFN_glBlendFunci = PFNGLBLENDFUNCIOESPROC;
using PFN_glBlendFuncSeparatei = PFNGLBLENDFUNCSEPARATEIOESPROC;
using PFN_glColorMaski = PFNGLCOLORMASKIOESPROC;
using PFN_glIsEnabledi = PFNGLISENABLEDIOESPROC;

#define DECLARE_OGL_SYMBOL(name) extern PFN_##name name;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

APPLY_OGL_DRAW_BUFFERS_INDEXED_SYMBOLS(DECLARE_OGL_SYMBOL);

//----------------------------------------------------------------------------------------------------------------------

inline GLenum to_GLDataUsage(Heap_type type)
{
    switch (type) {
        case Heap_type::local:
            return GL_STATIC_DRAW;
        case Heap_type::upload:
            return GL_DYNAMIC_DRAW;
        case Heap_type::readback:
            return GL_DYNAMIC_DRAW;
        default:
            throw std::runtime_error("invalid heap type");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline GLenum to_GLTextureTarget(Image_type type)
{
    switch (type) {
        case Image_type::two_dim:
            return GL_TEXTURE_2D;
        case Image_type::cube:
            return GL_TEXTURE_CUBE_MAP;
        default:
            throw std::runtime_error("invalid image type");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline GLenum to_GLInternalFormat(Format format)
{
    switch (format) {
        case Format::rgba8_unorm:
            return GL_RGBA8;
        case Format::bgra8_unorm:
            return GL_RGBA8;
        default:
            throw std::runtime_error("invalid format");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline GLenum to_GLFormat(Format format)
{
    switch (format) {
        case Format::rgba8_unorm:
            return GL_RGBA;
        case Format::bgra8_unorm:
            return GL_RGBA;
        default:
            throw std::runtime_error("invalid format");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline GLenum to_GLDataType(Format format)
{
    switch (format) {
        case Format::rgb8_unorm:
            return GL_UNSIGNED_BYTE;
        case Format::rgba8_unorm:
            return GL_UNSIGNED_BYTE;
        case Format::bgra8_unorm:
            return GL_UNSIGNED_BYTE;
        case Format::r32_float:
            return GL_FLOAT;
        case Format::rg32_float:
            return GL_FLOAT;
        case Format::rgb32_float:
            return GL_FLOAT;
        case Format::rgba32_float:
            return GL_FLOAT;
        case Format::d24_unorm_s8_uint:
            return GL_UNSIGNED_INT_24_8;
        default:
            throw std::runtime_error("invalid format");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline GLenum to_GLSamplerParam(Filter filter)
{
    switch (filter) {
        case Filter::nearest:
            return GL_NEAREST;
        case Filter::linear:
            return GL_LINEAR;
        default:
            throw std::runtime_error("invalid the filter");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline GLenum to_GLSamplerParam(Filter min_filter, Mip_filter mip_filter)
{
    if (Filter::nearest == min_filter) {
        switch (mip_filter) {
            case Mip_filter::nearest:
                return GL_NEAREST_MIPMAP_NEAREST;
            case Mip_filter::linear:
                return GL_NEAREST_MIPMAP_LINEAR;
            default:
                throw std::runtime_error("invalid the filter");
        }
    }
    else {
        switch (mip_filter) {
            case Mip_filter::nearest:
                return GL_LINEAR_MIPMAP_NEAREST;
            case Mip_filter::linear:
                return GL_LINEAR_MIPMAP_LINEAR;
            default:
                throw std::runtime_error("invalid the filter");
        }
    }

}

//----------------------------------------------------------------------------------------------------------------------

inline GLenum to_GLSamplerWrapMode(Address_mode mode)
{
    switch (mode) {
        case Address_mode::repeat:
            return GL_REPEAT;
        case Address_mode::clamp_to_edge:
            return GL_CLAMP_TO_EDGE;
        default:
            throw std::runtime_error("invalid the address mode");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline GLenum to_GLShaderType(Sc_lib::Shader_type type)
{
    switch (type) {
        case Sc_lib::Shader_type::vertex:
            return GL_VERTEX_SHADER;
        case Sc_lib::Shader_type ::fragment:
            return GL_FRAGMENT_SHADER;
        default:
            throw std::runtime_error("invalid shader type");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline GLenum to_GLPrimitiveMode(Topology topology)
{
    switch (topology) {
        case Topology::triangle_list:
            return GL_TRIANGLES;
        case Topology::triangle_strip:
            return GL_TRIANGLE_STRIP;
        case Topology::point:
            return GL_POINTS;
        default:
            throw std::runtime_error("invalid the topology");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline GLenum to_GLIndexType(Index_type type)
{
    switch (type) {
        case Index_type::uint16:
            return GL_UNSIGNED_SHORT;
        case Index_type::uint32:
            return GL_UNSIGNED_INT;
        default:
            throw std::runtime_error("invalid the index type");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline GLenum to_GLCullMode(Cull_mode mode)
{
    switch (mode) {
        case Cull_mode::front:
            return GL_FRONT;
        case Cull_mode::back:
            return GL_BACK;
        case Cull_mode::none:
            return GL_NONE;
        default:
            throw std::runtime_error("invalid the cull mode");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline GLenum to_GLCompareFunc(Compare_op op)
{
    switch (op) {
        case Compare_op::never:
            return GL_NEVER;
        case Compare_op::less:
            return GL_LESS;
        case Compare_op::greater:
            return GL_GREATER;
        case Compare_op::equal:
            return GL_EQUAL;
        case Compare_op::not_equal:
            return GL_NOTEQUAL;
        case Compare_op::less_or_equal:
            return GL_LEQUAL;
        case Compare_op::greater_or_equal:
            return GL_GEQUAL;
        case Compare_op::always:
            return GL_ALWAYS;
        default:
            throw std::runtime_error("invalid the compare op");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline GLenum to_GLBlendFactor(Blend_factor factor)
{
    switch (factor) {
        case Blend_factor::zero:
            return GL_ZERO;
        case Blend_factor::one:
            return GL_ONE;
        case Blend_factor::src_alpha:
            return GL_SRC_ALPHA;
        case Blend_factor::one_minus_src_alpha:
            return GL_ONE_MINUS_SRC_ALPHA;
        case Blend_factor::dst_alpha:
            return GL_DST_ALPHA;
        case Blend_factor::one_minus_dst_alpha:
            return GL_ONE_MINUS_DST_ALPHA;
        default:
            throw std::runtime_error("invalid the blend factor");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline GLenum to_GLBlendFunc(Blend_op op)
{
    switch (op) {
        case Blend_op::add:
            return GL_FUNC_ADD;
        case Blend_op::subtract:
            return GL_FUNC_SUBTRACT;
        case Blend_op::reverse_subtract:
            return GL_FUNC_REVERSE_SUBTRACT;
        case Blend_op::min:
            return GL_MIN;
        case Blend_op::max:
            return GL_MAX;
        default:
            throw std::runtime_error("invalid the blend op");
    }
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_OGL_LIB_GUARD