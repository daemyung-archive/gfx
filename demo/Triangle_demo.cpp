//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "Triangle_demo.h"

using namespace std;
using namespace Sc_lib;
using namespace Gfx_lib;

//----------------------------------------------------------------------------------------------------------------------

Triangle_demo::Triangle_demo() :
    Demo()
{
    init_pipeline_();
}

//----------------------------------------------------------------------------------------------------------------------

void Triangle_demo::render()
{
    if (!fence_->signaled())
        fence_->wait_signal();

    fence_->reset();
    cmd_buffer_->reset();

    Render_encoder_desc render_encoder_desc;

    render_encoder_desc.colors[0].image = swap_chain_->acquire();
    render_encoder_desc.colors[0].load_op = Load_op::clear;
    render_encoder_desc.colors[0].clear_value.r = 1.0f;
    render_encoder_desc.colors[0].clear_value.g = 1.0f;
    render_encoder_desc.colors[0].clear_value.b = 1.0f;
    render_encoder_desc.colors[0].clear_value.a = 1.0f;

    auto render_encoder = cmd_buffer_->create(render_encoder_desc);

    render_encoder->viewport({0.0f, 0.0f, 360.0f, 640.0f});
    render_encoder->pipeline(pipeline_.get());
    render_encoder->draw(3, 0);
    render_encoder->end();

    cmd_buffer_->end();

    device_->submit(cmd_buffer_.get(), fence_.get());
    swap_chain_->present();
}

//----------------------------------------------------------------------------------------------------------------------

void Triangle_demo::init_pipeline_()
{
    // create shaders.
    const vector<string> pathes {
        "../../../gfx/res/shader/triangle.vert",
        "../../../gfx/res/shader/triangle.frag"
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
