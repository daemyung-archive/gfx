//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include <gsl/gsl>
#include "Gfx_texture_demo.h"
#include "stb_lib_modules.h"

using namespace std;
using namespace gsl;
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

Gfx_texture_demo::Gfx_texture_demo(uint32_t w, uint32_t h) :
    window_ { nullptr },
    compiler_ {},
    device_ { nullptr },
    swap_chain_ { nullptr },
    frame_index_ { 0 },
    cmd_buffers_ { nullptr, nullptr, nullptr },
    fences_ { nullptr, nullptr, nullptr },
    vertex_buffer_ {}
{
    init_window_(w, h);
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_texture_demo::run()
{
    window_->run();
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_texture_demo::init_window_(uint32_t w, uint32_t h)
{
    Window_desc window_desc;

    window_desc.title = L"GFX Texture Demo";
    window_desc.extent = { w, h };

    window_ = make_unique<Window>(window_desc);

    assert(window_);
    window_->startup_signal.connect(this, &Gfx_texture_demo::on_startup_);
    window_->shutdown_signal.connect(this, &Gfx_texture_demo::on_shutdown_);
    window_->render_signal.connect(this, &Gfx_texture_demo::on_render_);
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_texture_demo::init_resources_()
{
    // create a device.
    device_ = Device::make();

    // create a swap chain.
    Swap_chain_desc swap_chain_desc;

#if __APPLE__
    swap_chain_desc.image_format = Format::bgra8_unorm;
#elif
    swap_chain_desc.image_format = Format::rgba8_unorm;
#endif
    swap_chain_desc.image_extent = window_->extent();
    swap_chain_desc.window = window_->window();

    swap_chain_ = device_->make(swap_chain_desc);

    // create cmd lists.
    for (auto& cmd_buffer : cmd_buffers_)
        cmd_buffer = device_->make_cmd_buffer();

    // create fences.
    for (auto& fence : fences_)
        fence = device_->make(Fence_desc { true });

    // create a vertex buffer.
    Buffer_desc vertex_buffer_desc;

    vertex_buffer_desc.data = static_cast<const void*>(&vertices[0]);
    vertex_buffer_desc.size = sizeof(Vertex) * vertices.size();

    vertex_buffer_ = device_->make(vertex_buffer_desc);

    // load a image file.
    int32_t w, h, c;
    auto data = stbi_load("../../../gfx/demo/dryforestground.jpg", &w, &h, &c, STBI_rgb_alpha);

    Buffer_desc staging_buffer_desc;

    staging_buffer_desc.data = data;
    staging_buffer_desc.size = w * h * STBI_rgb_alpha;

    auto staging_buffer = device_->make(staging_buffer_desc);

    Image_desc image_desc;

    image_desc.format = Format::rgba8_unorm;
    image_desc.extent = { narrow_cast<uint32_t>(w), narrow_cast<uint32_t>(h), 1 };

    image_ = device_->make(image_desc);

    auto cmd_buffer = device_->make_cmd_buffer();

    cmd_buffer->start();

    Buffer_image_copy_region copy_region;

    copy_region.buffer_row_size = w * STBI_rgb_alpha;
    copy_region.buffer_image_height = h;
    copy_region.image_extent = image_desc.extent;

    cmd_buffer->copy(staging_buffer.get(), image_.get(), copy_region);
    cmd_buffer->end();
    device_->submit(cmd_buffer.get());
    device_->wait_idle();

    // create a sampler
    Sampler_desc sampler_desc;

    sampler_ = device_->make(sampler_desc);

    // create a vertex shader.
    Shader_desc vertex_shader_desc;

    vertex_shader_desc.stage = Stage::vertex;
    vertex_shader_desc.src = compiler_.compile("../../../gfx/demo/gfx_texture.vert");

    auto vertex_shader = device_->make(vertex_shader_desc);

    // create a fragment shader.
    Shader_desc fragment_shader_desc;

    fragment_shader_desc.stage = Stage::vertex;
    fragment_shader_desc.src = compiler_.compile("../../../gfx/demo/gfx_texture.frag");

    auto fragment_shader = device_->make(fragment_shader_desc);

    // create a render pipeline.
    Pipeline_desc<Pipeline_type::render> render_pipeline_desc;

    render_pipeline_desc.vertex_state.attributes[0].binding = 0;
    render_pipeline_desc.vertex_state.attributes[0].format = Format::rgb32_float;
    render_pipeline_desc.vertex_state.attributes[1].binding = 0;
    render_pipeline_desc.vertex_state.attributes[1].format = Format::rg32_float;
    render_pipeline_desc.vertex_state.attributes[1].offset = sizeof(float) * 3;
    render_pipeline_desc.vertex_state.bindings[0].stride = sizeof(Vertex);
    render_pipeline_desc.vertex_shader_stage = vertex_shader.get();
    render_pipeline_desc.input_assembly_stage.topology = Topology::triangle_strip;
    render_pipeline_desc.fragment_shader_stage = fragment_shader.get();
    render_pipeline_desc.output_merger_stage.color_formats[0] = swap_chain_->image_format();

    render_pipeline_ = device_->make(render_pipeline_desc);
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_texture_demo::on_startup_()
{
    init_resources_();
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_texture_demo::on_shutdown_()
{
    device_->wait_idle();
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_texture_demo::on_render_()
{
    auto& fence = fences_[frame_index_];
    auto& cmd_buffer = cmd_buffers_[frame_index_];

    if (!fence->signaled())
        fence->wait_signal();

    cmd_buffer->start();

    Render_pass_state render_pass_state;

    render_pass_state.colors[0].image = swap_chain_->acquire();
    render_pass_state.colors[0].load_op = Load_op::clear;
    render_pass_state.colors[0].clear_value.r = 1.0f;
    render_pass_state.colors[0].clear_value.g = 1.0f;
    render_pass_state.colors[0].clear_value.b = 1.0f;
    render_pass_state.colors[0].clear_value.a = 1.0f;

    cmd_buffer->begin(render_pass_state);
    cmd_buffer->bind(vertex_buffer_.get(), 0);
    cmd_buffer->bind(image_.get(), static_cast<uint32_t>(Pipeline_stage::fragment),0);
    cmd_buffer->bind(sampler_.get(), static_cast<uint32_t>(Pipeline_stage::fragment), 0);
    cmd_buffer->bind(render_pipeline_.get());
    cmd_buffer->draw(4);
    cmd_buffer->end();

    cmd_buffer->stop();
    device_->submit(cmd_buffer.get(), fence.get());
    swap_chain_->present();

    frame_index_ = ++frame_index_ % 3;
}

//----------------------------------------------------------------------------------------------------------------------