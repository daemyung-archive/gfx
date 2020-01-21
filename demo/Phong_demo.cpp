//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "Phong_demo.h"

using namespace std;
using namespace glm;
using namespace Sc_lib;
using namespace Gfx_lib;

//----------------------------------------------------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------------------------------------------------

struct Matrix {
    mat4 model;
    mat4 normal;
    mat4 view;
    mat4 projection;
    mat4 mv;
    mat4 mvp;
};

//----------------------------------------------------------------------------------------------------------------------

struct Light {
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

//----------------------------------------------------------------------------------------------------------------------

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace

//----------------------------------------------------------------------------------------------------------------------

Phong_demo::Phong_demo() :
    Demo(),
    torus_ {0.3f, 0.7f, 50, 50},
    depth_stencil_image_ {nullptr},
    vertex_buffer_ {nullptr},
    vertex_input_ {},
    index_buffer_ {nullptr},
    index_count_ {0},
    shaders_ {nullptr, nullptr},
    pipeline_ {nullptr},
    uniform_buffers_ {},
    angle_ {0.0f}
{
    init_depth_stencil_image_();
    init_vertex_buffer_();
    init_index_buffer_();
    init_shaders_();
    init_pipeline_();
    init_uniform_buffers_();
}

//----------------------------------------------------------------------------------------------------------------------

void Phong_demo::render()
{
    if (!fence_->signaled())
        fence_->wait_signal();

    fence_->reset();
    cmd_buffer_->reset();

    auto matrix = reinterpret_cast<Matrix*>(uniform_buffers_["Matrix"]->map());

    matrix->model = rotate(mat4 {1.0f}, radians(angle_++), {0.0f, 1.0f, 0.0f});
    matrix->model = rotate(matrix->model, radians(-35.0f), {1.0f, 0.0f, 0.0f});
    matrix->model = rotate(matrix->model, radians(35.0f), {0.0f, 0.0f, 1.0f});
    matrix->normal = transpose(inverse(matrix->model));
    matrix->mv = matrix->view * matrix->model;
    matrix->mvp = matrix->projection * matrix->mv;

    uniform_buffers_["Matrix"]->unmap();

    Render_encoder_desc render_encoder_desc;

    render_encoder_desc.colors[0].image = swap_chain_->acquire();
    render_encoder_desc.colors[0].load_op = Load_op::clear;
    render_encoder_desc.colors[0].clear_value.r = 0.15f;
    render_encoder_desc.colors[0].clear_value.g = 0.15f;
    render_encoder_desc.colors[0].clear_value.b = 0.15f;
    render_encoder_desc.colors[0].clear_value.a = 1.0f;
    render_encoder_desc.depth_stencil.image = depth_stencil_image_.get();
    render_encoder_desc.depth_stencil.load_op = Load_op::clear;
    render_encoder_desc.depth_stencil.store_op = Store_op::dont_care;
    render_encoder_desc.depth_stencil.clear_value.d = 1.0f;
    render_encoder_desc.depth_stencil.clear_value.s = 0;

    auto render_encoder = cmd_buffer_->create(render_encoder_desc);

    render_encoder->viewport({0.0f, 0.0f, 360.0f, 640.0f});
    render_encoder->vertex_buffer(vertex_buffer_.get(), 0);
    render_encoder->index_buffer(index_buffer_.get(), Index_type::uint16);
    render_encoder->shader_buffer(Pipeline_stage::vertex_shader, uniform_buffers_["Matrix"].get(), 0, 0);
    render_encoder->shader_buffer(Pipeline_stage::vertex_shader, uniform_buffers_["Light"].get(), 0, 1);
    render_encoder->shader_buffer(Pipeline_stage::vertex_shader, uniform_buffers_["Material"].get(), 0, 2);
    render_encoder->pipeline(pipeline_.get());
    render_encoder->draw_indexed(index_count_);
    render_encoder->end();

    cmd_buffer_->end();

    device_->submit(cmd_buffer_.get(), fence_.get());
    swap_chain_->present();
}

//----------------------------------------------------------------------------------------------------------------------

void Phong_demo::init_depth_stencil_image_()
{
    Image_desc desc;

    desc.format = Format::d24_unorm_s8_uint;
    desc.extent = {360, 640, 1};

    depth_stencil_image_ = device_->create(desc);
}

//----------------------------------------------------------------------------------------------------------------------

void Phong_demo::init_vertex_buffer_()
{
    Buffer_desc buffer_desc;

    buffer_desc.data = &torus_.vertices[0];
    buffer_desc.size = sizeof(Vertex) * torus_.vertices.size();

    vertex_buffer_ = device_->create(buffer_desc);
    vertex_input_ = torus_.vertex_input;
}

//----------------------------------------------------------------------------------------------------------------------

void Phong_demo::init_index_buffer_()
{
    Buffer_desc buffer_desc;

    buffer_desc.data = &torus_.indices[0];
    buffer_desc.size = sizeof(uint16_t) * torus_.indices.size();

    index_buffer_ = device_->create(buffer_desc);
    index_count_ = torus_.indices.size();
}

//----------------------------------------------------------------------------------------------------------------------

void Phong_demo::init_shaders_()
{
    // create shaders.
    constexpr array<const char*, 2> pathes {
        "../../../gfx/res/shader/phong.vert",
        "../../../gfx/res/shader/phong.frag"
    };

    for (auto i = 0; i != 2; ++i) {
        Shader_desc shader_desc;

        shader_desc.type = static_cast<Shader_type>(i);
        shader_desc.src = compiler_.compile(pathes[i]);

        shaders_[i] = device_->create(shader_desc);
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Phong_demo::init_pipeline_()
{
    // create a pipeline.
    Pipeline_desc pipeline_desc;

    pipeline_desc.vertex_input = vertex_input_;
    pipeline_desc.vertex_shader = shaders_[0].get();
    pipeline_desc.depth_stencil.depth_test = true;
    pipeline_desc.depth_stencil.write_mask = true;
    pipeline_desc.fragment_shader = shaders_[1].get();
#if __APPLE__
    pipeline_desc.output_merger.color_formats[0] = Format::bgra8_unorm;
#else
    pipeline_desc.output_merger.color_formats[0] = Format::rgba8_unorm;
#endif
    pipeline_desc.output_merger.depth_stencil_format = Format::d24_unorm_s8_uint;

    pipeline_ = device_->create(pipeline_desc);
}

//----------------------------------------------------------------------------------------------------------------------

void Phong_demo::init_uniform_buffers_()
{
    auto signature = shaders_[0]->reflect();

    for (auto& buffer : signature.buffers) {
        Buffer_desc desc;

        desc.size = align_pow2(buffer.size, 64);
        desc.heap_type = Heap_type::upload;

        uniform_buffers_[buffer.name] = device_->create(desc);
    }

    vec3 cam_pos {0.0f, 0.0f, 2.0f};
    auto view_matrix = lookAt( cam_pos, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});

    auto light = reinterpret_cast<Light*>(uniform_buffers_["Light"]->map());

    light->position = view_matrix * vec4 {5.0f, 5.0f, 2.0f, 1.0f};
    light->ambient = {0.4f, 0.4f, 0.4f, 1.0f};
    light->diffuse = {1.0f, 1.0f, 1.0f, 1.0f};
    light->specular = {1.0f, 1.0f, 1.0f, 1.0f};

    uniform_buffers_["Light"]->unmap();

    auto matrix = reinterpret_cast<Matrix*>(uniform_buffers_["Matrix"]->map());

    matrix->model = mat4(1.0);
    matrix->normal = inverse(transpose(matrix->model));
    matrix->view = view_matrix;
    matrix->projection = glm::perspective(glm::radians(70.0f), 360.0f/640.0f, 0.3f, 100.0f);
    matrix->mv = matrix->view * matrix->model;
    matrix->mvp = matrix->projection * matrix->mv;

    uniform_buffers_["Matrix"]->unmap();

    auto material = reinterpret_cast<Material*>(uniform_buffers_["Material"]->map());

    material->ambient = {0.9f, 0.5f, 0.3f, 1.0f};
    material->diffuse = {0.9f, 0.5f, 0.3f, 1.0f};
    material->specular = {0.8f, 0.8f, 0.8f, 1.0f};
    material->shininess = 100.0f;

    uniform_buffers_["Material"]->unmap();
}

//----------------------------------------------------------------------------------------------------------------------
