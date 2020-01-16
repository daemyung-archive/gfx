//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "std_lib.h"
#include "ogl_lib.h"
#include "Ogl_cmd_buffer.h"
#include "Ogl_device.h"
#include "Ogl_buffer.h"
#include "Ogl_image.h"
#include "Ogl_sampler.h"
#include "Ogl_pipeline.h"

using namespace std;
using namespace Gfx_lib;

namespace {

//----------------------------------------------------------------------------------------------------------------------

inline auto component_count(Format format)
{
    switch (format) {
        case Format::r32_float:
            return 1;
        case Format::rg32_float:
            return 2;
        case Format::rgb32_float:
            return 3;
        case Format::rgba32_float:
            return 4;
        default:
            throw runtime_error("invalid format");
    }
}

//----------------------------------------------------------------------------------------------------------------------

inline void execute(function<void ()>& func)
{
    func();
}

//----------------------------------------------------------------------------------------------------------------------

inline Ogl_framebuffer_desc to_Ogl_framebuffer_desc(const Render_encoder_desc& desc)
{
    array<Ogl_image*, 4> color_images;

    for (auto i = 0; i != 4; ++i) {
        color_images[i] = static_cast<Ogl_image*>(desc.colors[i].image);
    }

    return {color_images, static_cast<Ogl_image*>(desc.depth_stencil.image)};
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Ogl_arg_table::Ogl_arg_table() :
    arg_buffers_ {},
    arg_textures_ {}
{
    clear();
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_arg_table::clear()
{
    arg_buffers_.fill({nullptr, 0});
    arg_textures_.fill({nullptr, nullptr});
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_arg_table::arg_buffer(const Ogl_arg_buffer& arg_buffer, uint32_t index)
{
    arg_buffers_[index] = arg_buffer;
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_arg_table::arg_texture(const Ogl_arg_texture& arg_texture, uint32_t index)
{
    arg_textures_[index] = arg_texture;
}

//----------------------------------------------------------------------------------------------------------------------

Ogl_render_encoder::Ogl_render_encoder(const Render_encoder_desc& desc, Ogl_device* device, Ogl_cmd_buffer* cmd_buffer) :
    Render_encoder {},
    device_ {device},
    cmd_buffer_ {cmd_buffer},
    framebuffer_ {nullptr},
    vertex_buffers_ {nullptr, nullptr},
    index_buffer_ {nullptr},
    index_type_ {Index_type::invalid},
    arg_table_ {},
    pipeline_ {nullptr},
    viewport_ {},
    scissor_ {},
    discards_ {}
{
    init_framebuffer_(desc);
    begin_render_pass_(desc);
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_render_encoder::end()
{
    end_render_pass_();

    for_each(cmds_, execute);
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_render_encoder::draw(uint32_t count, uint32_t first)
{
    auto input_assembly = pipeline_->input_assembly();

    cmds_.emplace_back([=]() {
        glDrawArrays(to_GLPrimitiveMode(input_assembly.topology), first, count);
    });
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_render_encoder::draw_indexed(uint32_t count, uint32_t first)
{
    auto input_assembly = pipeline_->input_assembly();

    cmds_.emplace_back([=]() {
       glDrawElements(to_GLPrimitiveMode(input_assembly.topology),
                      count, to_GLIndexType(index_type_), reinterpret_cast<void*>(first));
    });
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_render_encoder::vertex_buffer(Buffer* buffer, uint32_t index)
{
    auto buffer_impl = static_cast<Ogl_buffer*>(buffer);

    if (buffer_impl == vertex_buffers_[index])
        return;

    vertex_buffers_[index] = buffer_impl;

    if (!pipeline_)
        return;

    auto vertex_buffers = vertex_buffers_;
    auto vertex_input = pipeline_->vertex_input();

    cmds_.emplace_back([=]() {
        set_up_vertex_input_(vertex_buffers, vertex_input);
    });
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_render_encoder::index_buffer(Buffer* buffer, Index_type index_type)
{
    auto buffer_impl = static_cast<Ogl_buffer*>(buffer);

    if (buffer_impl == index_buffer_)
        return;

    index_buffer_ = buffer_impl;
    index_type_ = index_type;

    cmds_.emplace_back([=]() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_impl->buffer());
    });
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_render_encoder::shader_buffer(Pipeline_stage stage, Buffer* buffer, uint32_t offset, uint32_t index)
{
    auto buffer_impl = static_cast<Ogl_buffer*>(buffer);
    auto arg_buffer = arg_table_.arg_buffer(index);

    // skip if a shader buffer and offset are same.
    if (buffer_impl == arg_buffer.buffer && offset == arg_buffer.offset)
        return;

    cmds_.emplace_back([=] {
        glBindBuffer(GL_UNIFORM_BUFFER, buffer_impl->buffer());
        glBindBufferRange(GL_UNIFORM_BUFFER, index, buffer_impl->buffer(), offset, buffer_impl->size() - offset);
    });

    // update an arg tables.
    arg_table_.arg_buffer({buffer_impl, offset}, index);
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_render_encoder::shader_texture(Pipeline_stage stage, Image* image, Sampler* sampler, uint32_t index)
{
    auto image_impl = static_cast<Ogl_image*>(image);
    auto sampler_impl = static_cast<Ogl_sampler*>(sampler);
    auto arg_texture = arg_table_.arg_texture(index);

    // skip if a shader image and shader sampler are same.
    if (image_impl == arg_texture.image && sampler_impl == arg_texture.sampler)
        return;

    cmds_.emplace_back([=]() {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(to_GLTextureTarget(image_impl->type()), image_impl->texture());
        glBindSampler(index, sampler_impl->sampler());
    });

    // update an arg tables.
    arg_table_.arg_texture({image_impl, sampler_impl}, index);
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_render_encoder::pipeline(Pipeline* pipeline)
{
    auto pipeline_impl = static_cast<Ogl_pipeline*>(pipeline);

    if (pipeline_impl == pipeline_)
        return;

    pipeline_ = pipeline_impl;

    auto vertex_buffers = vertex_buffers_;

    cmds_.emplace_back([=]() {
        glUseProgram(pipeline_impl->program());
        set_up_vertex_input_(vertex_buffers, pipeline_impl->vertex_input());
        set_up_rasterization_(pipeline_impl->rasterization());
        set_up_depth_stencil_(pipeline_impl->depth_stencil());
        set_up_color_blend_(pipeline_impl->color_blend());
    });
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_render_encoder::viewport(const Viewport& viewport)
{
    if (viewport_ == viewport)
        return;

    cmds_.emplace_back([=]() {
        glViewport(viewport.x, viewport.y, viewport.w, viewport.h);
    });
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_render_encoder::scissor(const Scissor& scissor)
{
    if (scissor_ == scissor)
        return;

    cmds_.emplace_back([=]() {
        glScissor(scissor.x, scissor.y, scissor.w, scissor.h);
    });
}

//----------------------------------------------------------------------------------------------------------------------

Cmd_buffer* Ogl_render_encoder::cmd_buffer() const
{
    return cmd_buffer_;
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_render_encoder::init_framebuffer_(const Render_encoder_desc& desc)
{
    framebuffer_ = device_->framebuffer(to_Ogl_framebuffer_desc(desc));

    cmds_.emplace_back([=]() {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_->framebuffer());
    });
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_render_encoder::begin_render_pass_(const Render_encoder_desc& desc)
{
    cmds_.emplace_back([=]() {
        vector<GLenum> draw_buffers;

        for (GLuint i = 0; i != 4; ++i) {
            auto image = framebuffer_->color_image(i);

            if (image) {
                if (Image_type::swap_chain == image->type())
                    draw_buffers.push_back(GL_BACK);
                else
                    draw_buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
            }
        }

        glDrawBuffers(static_cast<GLsizei>(draw_buffers.size()), &draw_buffers[0]);

        for (GLuint i = 0; i != 4; ++i) {
            if (!framebuffer_->color_image(i))
                continue;

            auto& color = desc.colors[i];
            array<float, 4> clear_color { 0.0f, 0.0f, 0.0f, 0.0f };

            if (Load_op::clear == color.load_op) {
                clear_color[0] = color.clear_value.r;
                clear_color[1] = color.clear_value.g;
                clear_color[2] = color.clear_value.b;
                clear_color[3] = color.clear_value.a;
            }

            if (Load_op::dont_care != color.load_op)
                glClearBufferfv(GL_COLOR, i, &clear_color[0]);

            if (Store_op::dont_care == desc.colors[i].store_op)
                discards_.push_back(GL_COLOR_ATTACHMENT0 + i);
        }

        if (framebuffer_->depth_stencil()) {
            auto& depth_stencil = desc.depth_stencil;
            GLfloat d = 1.0f;
            GLint s = 0;

            if (Load_op::clear == depth_stencil.load_op) {
                d = depth_stencil.clear_value.d;
                s = depth_stencil.clear_value.s;
            }

            if (Load_op::dont_care != depth_stencil.load_op)
                glClearBufferfi(GL_DEPTH_STENCIL, 0, d, s);

            if (Store_op::dont_care == depth_stencil.store_op)
                discards_.push_back(GL_DEPTH_STENCIL_ATTACHMENT);
        }
    });
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_render_encoder::end_render_pass_()
{
    if (discards_.empty())
        return;

    cmds_.emplace_back([=]() {
        glInvalidateFramebuffer(GL_FRAMEBUFFER, static_cast<GLsizei>(discards_.size()), &discards_[0]);
        discards_.clear();
    });
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_render_encoder::set_up_vertex_input_(const std::array<Ogl_buffer*, 2>& vertex_buffers,
                                              const Vertex_input& vertex_input)
{
    for (auto i = 0; i != 2; ++i) {
        if (!vertex_buffers[i])
            continue;

        auto& binding = vertex_input.bindings[i];

        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[i]->buffer());

        for (GLuint j = 0; j != 16; ++j) {
            auto& attribute = vertex_input.attributes[j];

            if (i != attribute.binding)
                continue;

            if (Format::invalid == attribute.format) {
                glDisableVertexAttribArray(j);
            }
            else {
                glEnableVertexAttribArray(j);
                glVertexAttribPointer(j,
                                      component_count(attribute.format),
                                      to_GLDataType(attribute.format),
                                      GL_FALSE,
                                      binding.stride,
                                      reinterpret_cast<void*>(attribute.offset));
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_render_encoder::set_up_rasterization_(const Rasterization& rasterization)
{
    if (Cull_mode::none == rasterization.cull_mode) {
        glDisable(GL_CULL_FACE);
    }
    else {
        glEnable(GL_CULL_FACE);
        glCullFace(to_GLCullMode(rasterization.cull_mode));
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_render_encoder::set_up_depth_stencil_(const Depth_stencil& depth_stencil)
{
    if (depth_stencil.depth_test) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(to_GLCompareFunc(depth_stencil.depth_compare_op));
    }
    else {
        glDisable(GL_DEPTH_TEST);
    }

    glDepthMask(depth_stencil.write_mask);
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_render_encoder::set_up_color_blend_(const Color_blend& color_blend)
{
    for (GLuint i = 0; i != 4; ++ i) {
        auto& attachment = color_blend.attachments[i];

        if (attachment.blend) {
            glEnablei(GL_BLEND, i);
            glBlendFuncSeparatei(i,
                                 to_GLBlendFactor(attachment.src_rgb_blend_factor),
                                 to_GLBlendFactor(attachment.dst_rgb_blend_factor),
                                 to_GLBlendFactor(attachment.src_a_blend_factor),
                                 to_GLBlendFactor(attachment.dst_a_blend_factor));
            glBlendEquationSeparatei(i,
                                     to_GLBlendFunc(attachment.rgb_blend_op),
                                     to_GLBlendFunc(attachment.a_blend_op));
        }
        else {
            glDisablei(GL_BLEND, i);
        }

        glColorMaski(i,
                     attachment.write_mask & 0x8,
                     attachment.write_mask & 0x4,
                     attachment.write_mask & 0x2,
                     attachment.write_mask & 0x1);
    }

    glBlendColor(color_blend.constant[0], color_blend.constant[1], color_blend.constant[2], color_blend.constant[3]);
}

//----------------------------------------------------------------------------------------------------------------------

Ogl_blit_encoder::Ogl_blit_encoder(const Blit_encoder_desc& desc, Ogl_cmd_buffer* cmd_buffer) :
    Blit_encoder {},
    cmd_buffer_ {cmd_buffer}
{
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_blit_encoder::copy(Buffer* src_buffer, Buffer* dst_buffer, const Buffer_copy_region& region)
{
    auto src_buffer_impl = static_cast<Ogl_buffer*>(src_buffer);
    auto dst_buffer_impl = static_cast<Ogl_buffer*>(dst_buffer);

    cmds_.emplace_back([=]() {
        auto contents = static_cast<uint8_t*>(src_buffer_impl->map());

        glBindBuffer(GL_COPY_WRITE_BUFFER, dst_buffer_impl->buffer());
        glBufferSubData(GL_COPY_WRITE_BUFFER, region.dst_offset, region.size, contents + region.src_offset);
        src_buffer_impl->unmap();
    });
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_blit_encoder::copy(Buffer* src_buffer, Image* dst_image, const Buffer_image_copy_region& region)
{
    auto src_buffer_impl = static_cast<Ogl_buffer*>(src_buffer);
    auto dst_image_impl = static_cast<Ogl_image*>(dst_image);

    cmds_.emplace_back([=]() {
        auto contents = src_buffer_impl->map();

        glBindTexture(GL_TEXTURE_2D, dst_image_impl->texture());
        glTexSubImage2D(GL_TEXTURE_2D,
                        region.image_subresource.mip_level,
                        region.image_offset.x,
                        region.image_offset.y,
                        region.image_extent.w,
                        region.image_extent.h,
                        to_GLFormat(dst_image_impl->format()),
                        to_GLDataType(dst_image_impl->format()),
                        contents);
        src_buffer_impl->unmap();
    });
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_blit_encoder::copy(Image* src_buffer, Buffer* dst_image, const Buffer_image_copy_region& region)
{
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_blit_encoder::end()
{
    for_each(cmds_, execute);
}

//----------------------------------------------------------------------------------------------------------------------

Cmd_buffer* Ogl_blit_encoder::cmd_buffer() const
{
    return cmd_buffer_;
}

//----------------------------------------------------------------------------------------------------------------------

Ogl_cmd_buffer::Ogl_cmd_buffer(Ogl_device* device) :
    Cmd_buffer {},
    device_ {device}
{
}

//----------------------------------------------------------------------------------------------------------------------

Ogl_cmd_buffer::~Ogl_cmd_buffer()
{
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Render_encoder> Ogl_cmd_buffer::create(const Render_encoder_desc& desc)
{
    return make_unique<Ogl_render_encoder>(desc, device_, this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Blit_encoder> Ogl_cmd_buffer::create(const Blit_encoder_desc& desc)
{
    return make_unique<Ogl_blit_encoder>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_cmd_buffer::end()
{
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_cmd_buffer::reset()
{
}

//----------------------------------------------------------------------------------------------------------------------

Device* Ogl_cmd_buffer::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib