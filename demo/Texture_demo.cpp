//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include <gsl/gsl>
#include "stb_lib.h"
#include "Texture_demo.h"

using namespace std;
using namespace gsl;
using namespace Sc_lib;
using namespace Gfx_lib;

namespace {

//----------------------------------------------------------------------------------------------------------------------

struct Vertex {
    float x, y, z;
    float u, v;
};

//----------------------------------------------------------------------------------------------------------------------

const vector<Vertex> vertices = {
    { -0.7f, -0.7f, 0.0f, 0.0f, 1.0f },
    {  0.7f, -0.7f, 0.0f, 1.0f, 1.0f },
    { -0.7f,  0.7f, 0.0f, 0.0f, 0.0f },
    {  0.7f,  0.7f, 0.0f, 1.0f, 0.0f }
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace

//----------------------------------------------------------------------------------------------------------------------

Texture_demo::Texture_demo() :
    vertex_buffer_ {},
    image_ {},
    sampler_ {},
    pipeline_ {}
{
    init_vertex_buffer_();
    init_image_();
    init_sampler_();
    init_pipeline_();
}

//----------------------------------------------------------------------------------------------------------------------

void Texture_demo::render()
{
    if (!fence_->signaled())
        fence_->wait_signal();

    fence_->reset();
    cmd_buffer_->reset();

    Render_encoder_desc render_pass;

    render_pass.colors[0].image = swap_chain_->acquire();
    render_pass.colors[0].load_op = Load_op::clear;
    render_pass.colors[0].clear_value.r = 1.0f;
    render_pass.colors[0].clear_value.g = 1.0f;
    render_pass.colors[0].clear_value.b = 1.0f;
    render_pass.colors[0].clear_value.a = 1.0f;

    auto encoder = cmd_buffer_->create(render_pass);

    encoder->viewport({0.0f, 0.0f, 360.0f, 640.0f});
    encoder->vertex_buffer(vertex_buffer_.get(), 0);
    encoder->shader_texture(Pipeline_stage::fragment_shader, image_.get(), sampler_.get(), 0);
    encoder->pipeline(pipeline_.get());
    encoder->draw(4);
    encoder->end();

    cmd_buffer_->end();

    device_->submit(cmd_buffer_.get(), fence_.get());
    swap_chain_->present();
}

//----------------------------------------------------------------------------------------------------------------------

void Texture_demo::init_vertex_buffer_()
{
    // create a vertex buffer.
    Buffer_desc buffer_desc;

    buffer_desc.data = static_cast<const void*>(&vertices[0]);
    buffer_desc.size = sizeof(Vertex) * vertices.size();

    vertex_buffer_ = device_->create(buffer_desc);
}

//----------------------------------------------------------------------------------------------------------------------

void Texture_demo::init_image_()
{
    // load a image file.
    int32_t w, h, c;
    auto data = stbi_load("../../../gfx/res/image/statue.jpg", &w, &h, &c, STBI_rgb_alpha);

    Buffer_desc staging_buffer_desc;

    staging_buffer_desc.data = data;
    staging_buffer_desc.size = w * h * STBI_rgb_alpha;

    auto staging_buffer = device_->create(staging_buffer_desc);

    stbi_image_free(data);

    Image_desc image_desc;

    image_desc.format = Format::rgba8_unorm;
    image_desc.extent = { narrow_cast<uint32_t>(w), narrow_cast<uint32_t>(h), 1 };

    image_ = device_->create(image_desc);

    auto cmd_buffer = device_->create(Cmd_buffer_desc {});

    Buffer_image_copy_region copy_region;

    copy_region.buffer_row_size = w * STBI_rgb_alpha;
    copy_region.buffer_image_height = h;
    copy_region.image_extent = image_desc.extent;

    auto encoder = cmd_buffer->create(Blit_encoder_desc {});

    encoder->copy(staging_buffer.get(), image_.get(), copy_region);
    encoder->end();
    cmd_buffer->end();

    device_->submit(cmd_buffer.get());
    device_->wait_idle();
}

//----------------------------------------------------------------------------------------------------------------------

void Texture_demo::init_sampler_()
{
    sampler_ = device_->create(Sampler_desc {});
}

//----------------------------------------------------------------------------------------------------------------------

void Texture_demo::init_pipeline_()
{
    // create shaders.
    const vector<string> pathes {
        "../../../gfx/res/shader/texture.vert",
        "../../../gfx/res/shader/texture.frag"
    };

    array<unique_ptr<Shader>, 2> shaders;

    for (auto i = 0; i != 2; ++i) {
        Shader_desc shader_desc;

        shader_desc.type = static_cast<Shader_type>(i);
        shader_desc.src = compiler_.compile(pathes[i]);

        shaders[i] = device_->create(shader_desc);
    }

    // create a pipeline.
    Pipeline_desc pipeline_desc;

    pipeline_desc.vertex_input.attributes[0].binding = 0;
    pipeline_desc.vertex_input.attributes[0].format = Format::rgb32_float;
    pipeline_desc.vertex_input.attributes[1].binding = 0;
    pipeline_desc.vertex_input.attributes[1].format = Format::rg32_float;
    pipeline_desc.vertex_input.attributes[1].offset = sizeof(float) * 3;
    pipeline_desc.vertex_input.bindings[0].stride = sizeof(Vertex);
    pipeline_desc.input_assembly.topology = Topology::triangle_strip;
    pipeline_desc.vertex_shader = shaders[0].get();
    pipeline_desc.fragment_shader = shaders[1].get();
#if __APPLE__
    pipeline_desc.output_merger.color_formats[0] = Format::bgra8_unorm;
#else
    pipeline_desc.output_merger.color_formats[0] = Format::rgba8_unorm;
#endif

    pipeline_ = device_->create(pipeline_desc);
}

//----------------------------------------------------------------------------------------------------------------------
