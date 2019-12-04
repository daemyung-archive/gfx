//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "Gfx_triangle_demo.h"

using namespace std;
using namespace Gfx_lib;

namespace {

//----------------------------------------------------------------------------------------------------------------------

struct Vertex {
    float x, y, z;
    float r, g, b;
};

//----------------------------------------------------------------------------------------------------------------------

const vector<Vertex> vertices = {
    {  0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f },
    { -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f },
    {  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f }
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace

//----------------------------------------------------------------------------------------------------------------------

Gfx_triangle_demo::Gfx_triangle_demo(uint32_t w, uint32_t h) :
    window_ { nullptr },
    compiler_ {},
    device_ { nullptr },
    swap_chain_ { nullptr },
    frame_index_ { 0 },
    cmd_lists_ { nullptr, nullptr, nullptr },
    fences_ { nullptr, nullptr, nullptr },
    vertex_buffer_ {}
{
    init_window_(w, h);
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_triangle_demo::run()
{
    window_->run();
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_triangle_demo::init_window_(uint32_t w, uint32_t h)
{
    Window_desc window_desc;

    window_desc.title = L"GFX Triangle Demo";
    window_desc.extent = { w, h };

    window_ = make_unique<Window>(window_desc);

    assert(window_);
    window_->startup_signal.connect(std::bind(&Gfx_triangle_demo::on_startup_, this));
    window_->startup_signal.connect(std::bind(&Gfx_triangle_demo::on_shutdown_, this));
    window_->startup_signal.connect(std::bind(&Gfx_triangle_demo::on_render_, this));
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_triangle_demo::init_resources_()
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
    for (auto& cmd_list : cmd_lists_)
        cmd_list = device_->make_cmd_list();

    // create fences.
    for (auto& fence : fences_)
        fence = device_->make(Fence_desc { true });

    // create a vertex buffer.
    Buffer_desc vertex_buffer_desc;

    vertex_buffer_desc.data = static_cast<const void*>(&vertices[0]);
    vertex_buffer_desc.size = sizeof(Vertex) * vertices.size();

    vertex_buffer_ = device_->make(vertex_buffer_desc);

    // create a vertex shader.
    Shader_desc vertex_shader_desc;

    vertex_shader_desc.stage = Stage::vertex;
    vertex_shader_desc.src = compiler_.compile("../../../gfx/demo/gfx_triangle.vert");

    auto vertex_shader = device_->make(vertex_shader_desc);

    // create a fragment shader.
    Shader_desc fragment_shader_desc;

    fragment_shader_desc.stage = Stage::vertex;
    fragment_shader_desc.src = compiler_.compile("../../../gfx/demo/gfx_triangle.frag");

    auto fragment_shader = device_->make(fragment_shader_desc);

    // create a render pipeline.
    Pipeline_desc<Pipeline_type::render> render_pipeline_desc;

    render_pipeline_desc.vertex_state.attributes[0].binding = 0;
    render_pipeline_desc.vertex_state.attributes[0].format = Format::rgb32_float;
    render_pipeline_desc.vertex_state.attributes[1].binding = 0;
    render_pipeline_desc.vertex_state.attributes[1].format = Format::rgb32_float;
    render_pipeline_desc.vertex_state.attributes[1].offset = sizeof(float) * 3;
    render_pipeline_desc.vertex_state.bindings[0].stride = sizeof(Vertex);
    render_pipeline_desc.vertex_shader_stage = vertex_shader.get();
    render_pipeline_desc.fragment_shader_stage = fragment_shader.get();
    render_pipeline_desc.output_merger_stage.color_formats[0] = swap_chain_->image_format();

    render_pipeline_ = device_->make(render_pipeline_desc);
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_triangle_demo::on_startup_()
{
    init_resources_();
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_triangle_demo::on_shutdown_()
{
    device_->wait_idle();
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_triangle_demo::on_render_()
{
    auto& fence = fences_[frame_index_];
    auto& cmd_list = cmd_lists_[frame_index_];

    if (!fence->signaled())
        fence->wait_signal();

    cmd_list->start();

    Render_pass_state render_pass_state;

    render_pass_state.colors[0].image = swap_chain_->acquire();
    render_pass_state.colors[0].load_op = Load_op::clear;
    render_pass_state.colors[0].clear_value.r = 1.0f;
    render_pass_state.colors[0].clear_value.g = 1.0f;
    render_pass_state.colors[0].clear_value.b = 1.0f;
    render_pass_state.colors[0].clear_value.a = 1.0f;

    cmd_list->begin(render_pass_state);
    cmd_list->bind(vertex_buffer_.get(), 0);
    cmd_list->bind(render_pipeline_.get());
    cmd_list->draw(3);
    cmd_list->end();

    cmd_list->stop();
    device_->submit(cmd_list.get(), fence.get());
    swap_chain_->present();

    frame_index_ = ++frame_index_ % 3;
}

//----------------------------------------------------------------------------------------------------------------------
