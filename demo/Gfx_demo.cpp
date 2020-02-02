//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include <iostream>
#include <glm/ext.hpp>
#include <imgui.h>
#include "Gfx_demo.h"
#include "util.h"

using namespace std;
using namespace glm;
using namespace Sc_lib;
using namespace Gfx_lib;

namespace {

//----------------------------------------------------------------------------------------------------------------------

struct Imgui_info {
    vec2 translation;
    vec2 scale;
};

//----------------------------------------------------------------------------------------------------------------------

struct Matrix_info {
    mat4 model;
    mat4 view;
    mat4 projection;
    mat4 mv;
    mat4 mvp;
    mat4 normal;
};

//----------------------------------------------------------------------------------------------------------------------

struct Light_info {
    vec4 translation;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

//----------------------------------------------------------------------------------------------------------------------

struct Material_info {
    vec4 ambient;
    vec4 diffuse;
    vec3 specular;
    float shininess;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace

//----------------------------------------------------------------------------------------------------------------------

Gfx_demo::Gfx_demo() :
    cfgs_ {}
{
    init_cfgs_();
    init_device_();
    init_light_resources_();
}

//----------------------------------------------------------------------------------------------------------------------

Gfx_demo::~Gfx_demo()
{
    device_->wait_idle();

    fini_imgui_();
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_demo::connect(Platform_lib::Window* window)
{
    init_swap_chain_(window);
    init_cmd_buffer_();
    init_fence_();
    init_imgui_();
    init_imgui_resources_();
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_demo::render()
{
    if (!fence_->signaled())
        fence_->wait_signal();

    fence_->reset();
    cmd_buffer_->reset();

    record_light_render_pass_();
    record_present_render_pass_();

    cmd_buffer_->end();

    device_->submit(cmd_buffer_.get(), fence_.get());
    swap_chain_->present();
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_demo::touch_down()
{
    ImGui::GetIO().MouseDown[0] = true;
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_demo::touch_move(float x, float y)
{
    ImGui::GetIO().MousePos = {x, y};
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_demo::touch_up()
{
    ImGui::GetIO().MouseDown[0] = false;
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_demo::init_cfgs_()
{
    cfgs_.cube.rotation.x = 45.0f;
    cfgs_.cube.rotation.y = 45.0f;
    cfgs_.cube.ambient = {0.135f, 0.2225f, 0.1575f};
    cfgs_.cube.diffuse = {0.54f, 0.89f, 0.63f};
    cfgs_.cube.specular = {0.316228f, 0.316228f, 0.316228f};
    cfgs_.cube.shininess = 0.1f * 128.0f;

    cfgs_.torus.translation = {1.75f, 0.0f, 0.0f};
    cfgs_.torus.rotation.x = 125.0f;
    cfgs_.torus.rotation.y = 45.0f;
    cfgs_.torus.style = 1;
    cfgs_.torus.ambient = {0.19225f, 0.19225f, 0.19225f};
    cfgs_.torus.diffuse = {0.50754f, 0.50754f, 0.50754f};
    cfgs_.torus.specular = {0.508273f, 0.508273f, 0.508273f};
    cfgs_.torus.shininess = 0.4f * 128.0f;

    cfgs_.sphere.translation = {-1.75f, 0.0f, 0.0f};
    cfgs_.sphere.style = 2;
    cfgs_.sphere.ambient = {0.24725f, 0.1995f, 0.0745f};
    cfgs_.sphere.diffuse = {0.75164f, 0.60648f, 0.22648f};
    cfgs_.sphere.specular = {0.628281f, 0.555802f, 0.366065f};
    cfgs_.sphere.shininess = 0.4f * 128.0f;
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_demo::init_device_()
{
    try {
        device_ = Device::create();
    }
    catch(exception& e) {
        throw runtime_error("fail to create a demo");
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_demo::init_light_resources_()
{
    Plane plane {1.0f, 1.0f};

    try {
        Buffer_desc buffer_desc;

        buffer_desc.data = &plane.vertices[0];
        buffer_desc.size = sizeof(Vertex) * plane.vertices.size();

        buffers_["plane_vertex"] = device_->create(buffer_desc);
    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    try {
        Buffer_desc buffer_desc;

        buffer_desc.data = &plane.indices[0];
        buffer_desc.size = sizeof(uint16_t) * plane.indices.size();

        buffers_["plane_index"] = device_->create(buffer_desc);
        draw_counts_["plane"] = plane.indices.size();
    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    Cube cube {1.0f};

    try {
        Buffer_desc buffer_desc;

        buffer_desc.data = &cube.vertices[0];
        buffer_desc.size = sizeof(Vertex) * cube.vertices.size();

        buffers_["cube_vertex"] = device_->create(buffer_desc);
    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    try {
        Buffer_desc buffer_desc;

        buffer_desc.data = &cube.indices[0];
        buffer_desc.size = sizeof(uint16_t) * cube.indices.size();

        buffers_["cube_index"] = device_->create(buffer_desc);
        draw_counts_["cube"] = cube.indices.size();
    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    Torus torus {0.3f, 0.6f, 64, 64};

    try {
        Buffer_desc buffer_desc;

        buffer_desc.data = &torus.vertices[0];
        buffer_desc.size = sizeof(Vertex) * torus.vertices.size();

        buffers_["torus_vertex"] = device_->create(buffer_desc);
    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    try {
        Buffer_desc buffer_desc;

        buffer_desc.data = &torus.indices[0];
        buffer_desc.size = sizeof(uint16_t) * torus.indices.size();

        buffers_["torus_index"] = device_->create(buffer_desc);
        draw_counts_["torus"] = torus.indices.size();
    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    Sphere sphere {0.7f, 64, 64};

    try {
        Buffer_desc buffer_desc;

        buffer_desc.data = &sphere.vertices[0];
        buffer_desc.size = sizeof(Vertex) * sphere.vertices.size();

        buffers_["sphere_vertex"] = device_->create(buffer_desc);
    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    try {
        Buffer_desc buffer_desc;

        buffer_desc.data = &sphere.indices[0];
        buffer_desc.size = sizeof(uint16_t) * sphere.indices.size();

        buffers_["sphere_index"] = device_->create(buffer_desc);
        draw_counts_["sphere"] = sphere.indices.size();
    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    try {
        Buffer_desc buffer_desc;

        buffer_desc.size = align_pow2(sizeof(Matrix_info), 256) * 7;

        buffers_["matrix_info"] = device_->create(buffer_desc);
    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    try {
        Buffer_desc buffer_desc;

        buffer_desc.size = sizeof(Light_info);

        buffers_["light_info"] = device_->create(buffer_desc);
    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    try {
        Buffer_desc buffer_desc;

        buffer_desc.size = align_pow2(sizeof(Material_info), 256) * 7;

        buffers_["material_info"] = device_->create(buffer_desc);
    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    try {
        Image_desc image_desc;

        image_desc.format = Format::rgba8_unorm;
        image_desc.extent = {1280, 720, 1};

        auto light_image = device_->create(image_desc);

        images_["light_color"] = move(light_image);
    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    try {
        Image_desc desc;

        desc.format = Format::d24_unorm_s8_uint;
        desc.extent = {1280, 720, 1};

        images_["light_depth_stencil"] = device_->create(desc);
    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    try {
        Sampler_desc desc;

        samplers_["light_linear"] = device_->create(desc);
    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    try {
        // create shaders.
        const vector<string> pathes {
#if defined(__ANDROID__)
            "/sdcard/Android/data/com.ff.gfx_demo/files/lamp.vert",
            "/sdcard/Android/data/com.ff.gfx_demo/files/lamp.frag"
#else
            "../../../gfx/res/shader/lamp.vert",
            "../../../gfx/res/shader/lamp.frag"
#endif
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

        pipeline_desc.vertex_input = torus.vertex_input;
        pipeline_desc.vertex_shader = shaders[0].get();
        pipeline_desc.depth_stencil.depth_test = true;
        pipeline_desc.fragment_shader = shaders[1].get();
        pipeline_desc.output_merger.color_formats[0] = Format::rgba8_unorm;
        pipeline_desc.output_merger.depth_stencil_format = Format::d24_unorm_s8_uint;

        pipelines_["lamp"] = device_->create(pipeline_desc);

    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    try {
        // create shaders.
        const vector<string> pathes {
#if defined(__ANDROID__)
            "/sdcard/Android/data/com.ff.gfx_demo/files/flat.vert",
            "/sdcard/Android/data/com.ff.gfx_demo/files/flat.frag"
#else
            "../../../gfx/res/shader/flat.vert",
            "../../../gfx/res/shader/flat.frag"
#endif
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

        pipeline_desc.vertex_input = torus.vertex_input;
        pipeline_desc.vertex_shader = shaders[0].get();
        pipeline_desc.depth_stencil.depth_test = true;
        pipeline_desc.fragment_shader = shaders[1].get();
        pipeline_desc.output_merger.color_formats[0] = Format::rgba8_unorm;
        pipeline_desc.output_merger.depth_stencil_format = Format::d24_unorm_s8_uint;

        pipelines_["flat"] = device_->create(pipeline_desc);

    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    try {
        // create shaders.
        const vector<string> pathes {
#if defined(__ANDROID__)
            "/sdcard/Android/data/com.ff.gfx_demo/files/gouraud.vert",
            "/sdcard/Android/data/com.ff.gfx_demo/files/gouraud.frag"
#else
            "../../../gfx/res/shader/gouraud.vert",
            "../../../gfx/res/shader/gouraud.frag"
#endif
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

        pipeline_desc.vertex_input = torus.vertex_input;
        pipeline_desc.vertex_shader = shaders[0].get();
        pipeline_desc.depth_stencil.depth_test = true;
        pipeline_desc.fragment_shader = shaders[1].get();
        pipeline_desc.output_merger.color_formats[0] = Format::rgba8_unorm;
        pipeline_desc.output_merger.depth_stencil_format = Format::d24_unorm_s8_uint;

        pipelines_["gouraud"] = device_->create(pipeline_desc);

    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    try {
        // create shaders.
        const vector<string> pathes {
#if defined(__ANDROID__)
            "/sdcard/Android/data/com.ff.gfx_demo/files/phong.vert",
            "/sdcard/Android/data/com.ff.gfx_demo/files/phong.frag"
#else
            "../../../gfx/res/shader/phong.vert",
            "../../../gfx/res/shader/phong.frag"
#endif
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

        pipeline_desc.vertex_input = torus.vertex_input;
        pipeline_desc.vertex_shader = shaders[0].get();
        pipeline_desc.depth_stencil.depth_test = true;
        pipeline_desc.fragment_shader = shaders[1].get();
        pipeline_desc.output_merger.color_formats[0] = Format::rgba8_unorm;
        pipeline_desc.output_merger.depth_stencil_format = Format::d24_unorm_s8_uint;

        pipelines_["phong"] = device_->create(pipeline_desc);
    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_demo::record_light_render_pass_()
{
    auto light_info = reinterpret_cast<Light_info*>(buffers_["light_info"]->map());

    light_info->translation = {cfgs_.light.translation, 1.0};
    light_info->ambient = {cfgs_.light.ambient, 1.0};
    light_info->diffuse = {cfgs_.light.diffuse, 1.0};
    light_info->specular = {cfgs_.light.specular, 1.0};

    auto view = lookAt(cfgs_.camera.translation, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});

    auto projection = perspective(radians(cfgs_.camera.fov),
                                  cfgs_.camera.aspect,
                                  cfgs_.camera.near, cfgs_.camera.far);

    buffers_["light_info"]->unmap();

    auto matrix_info_contents = static_cast<uint8_t*>(buffers_["matrix_info"]->map());

    // update light matrix info.
    auto lamp_matrix_info = reinterpret_cast<Matrix_info*>(matrix_info_contents);

    lamp_matrix_info->model = translate(mat4 {1.0}, cfgs_.light.translation);
    lamp_matrix_info->model = scale(lamp_matrix_info->model, {0.2f, 0.2f, 0.2f});
    lamp_matrix_info->view = view;
    lamp_matrix_info->projection = projection;
    lamp_matrix_info->mv = view * lamp_matrix_info->model;
    lamp_matrix_info->mvp = projection * lamp_matrix_info->mv;
    lamp_matrix_info->normal = inverse(transpose(lamp_matrix_info->mv));

    auto bottom_plane_matrix_info = reinterpret_cast<Matrix_info*>(matrix_info_contents + 512 * 2);

    bottom_plane_matrix_info->model = translate(mat4 {1.0}, {0.0f, -1.0f, 0.0f});
    bottom_plane_matrix_info->model = rotate(bottom_plane_matrix_info->model, radians(-90.0f), {1.0f, 0.0f, 0.0f});
    bottom_plane_matrix_info->model = scale(bottom_plane_matrix_info->model, {10.0f, 10.0f, 1.0f});
    bottom_plane_matrix_info->view = view;
    bottom_plane_matrix_info->projection = projection;
    bottom_plane_matrix_info->mv = view * bottom_plane_matrix_info->model;
    bottom_plane_matrix_info->mvp = projection * bottom_plane_matrix_info->mv;
    bottom_plane_matrix_info->normal = inverse(transpose(bottom_plane_matrix_info->mv));

    auto far_plane_matrix_info = reinterpret_cast<Matrix_info*>(matrix_info_contents + 512 * 3);

    far_plane_matrix_info->model = translate(mat4 {1.0}, {0.0f, 4.0f, -5.0f});
    far_plane_matrix_info->model = scale(far_plane_matrix_info->model, {10.0f, 10.0f, 1.0f});
    far_plane_matrix_info->view = view;
    far_plane_matrix_info->projection = projection;
    far_plane_matrix_info->mv = view * far_plane_matrix_info->model;
    far_plane_matrix_info->mvp = projection * far_plane_matrix_info->mv;
    far_plane_matrix_info->normal = inverse(transpose(far_plane_matrix_info->mv));

    // update cube matrix info.
    auto cube_matrix_info = reinterpret_cast<Matrix_info*>(matrix_info_contents + 512 * 4);

    if (cfgs_.cube.animation)
        cfgs_.cube.rotation.z += 1.0f;

    cube_matrix_info->model = translate(mat4 {1.0}, cfgs_.cube.translation);
    cube_matrix_info->model = rotate(cube_matrix_info->model, radians(cfgs_.cube.rotation.x), {1.0f, 0.0f, 0.0f});
    cube_matrix_info->model = rotate(cube_matrix_info->model, radians(cfgs_.cube.rotation.y), {0.0f, 1.0f, 0.0f});
    cube_matrix_info->model = rotate(cube_matrix_info->model, radians(cfgs_.cube.rotation.z), {0.0f, 0.0f, 1.0f});
    cube_matrix_info->model = scale(cube_matrix_info->model, cfgs_.cube.scale);
    cube_matrix_info->view = view;
    cube_matrix_info->projection = projection;
    cube_matrix_info->mv = view * cube_matrix_info->model;
    cube_matrix_info->mvp = projection * cube_matrix_info->mv;
    cube_matrix_info->normal = inverse(transpose(cube_matrix_info->mv));

    // update torus matrix info.
    auto torus_matrix_info = reinterpret_cast<Matrix_info*>(matrix_info_contents + 512 * 5);

    if (cfgs_.torus.animation)
        cfgs_.torus.rotation.y += 1.0f;

    torus_matrix_info->model = translate(mat4 {1.0}, cfgs_.torus.translation);
    torus_matrix_info->model = rotate(torus_matrix_info->model, radians(cfgs_.torus.rotation.x), {1.0f, 0.0f, 0.0f});
    torus_matrix_info->model = rotate(torus_matrix_info->model, radians(cfgs_.torus.rotation.y), {0.0f, 1.0f, 0.0f});
    torus_matrix_info->model = rotate(torus_matrix_info->model, radians(cfgs_.torus.rotation.z), {0.0f, 0.0f, 1.0f});
    torus_matrix_info->model = scale(torus_matrix_info->model, cfgs_.torus.scale);
    torus_matrix_info->view = view;
    torus_matrix_info->projection = projection;
    torus_matrix_info->mv = view * torus_matrix_info->model;
    torus_matrix_info->mvp = projection * torus_matrix_info->mv;
    torus_matrix_info->normal = inverse(transpose(torus_matrix_info->mv));

    // update torus matrix info.
    auto sphere_matrix_info = reinterpret_cast<Matrix_info*>(matrix_info_contents + 512 * 6);

    if (cfgs_.sphere.animation)
        cfgs_.sphere.rotation.z += 1.0f;

    sphere_matrix_info->model = translate(mat4 {1.0}, cfgs_.sphere.translation);
    sphere_matrix_info->model = rotate(sphere_matrix_info->model, radians(cfgs_.sphere.rotation.x), {1.0f, 0.0f, 0.0f});
    sphere_matrix_info->model = rotate(sphere_matrix_info->model, radians(cfgs_.sphere.rotation.y), {0.0f, 1.0f, 0.0f});
    sphere_matrix_info->model = rotate(sphere_matrix_info->model, radians(cfgs_.sphere.rotation.z), {0.0f, 0.0f, 1.0f});
    sphere_matrix_info->model = scale(sphere_matrix_info->model, cfgs_.sphere.scale);
    sphere_matrix_info->view = view;
    sphere_matrix_info->projection = projection;
    sphere_matrix_info->mv = view * sphere_matrix_info->model;
    sphere_matrix_info->mvp = projection * sphere_matrix_info->mv;
    sphere_matrix_info->normal = inverse(transpose(sphere_matrix_info->mv));

    buffers_["matrix_info"]->unmap();

    auto material_info_contents = reinterpret_cast<uint8_t*>(buffers_["material_info"]->map());
    auto bottom_plane_material_info = reinterpret_cast<Material_info*>(material_info_contents + 256 * 2);

    bottom_plane_material_info->ambient = {0.2125f, 0.1275f, 0.054f, 0.0f};
    bottom_plane_material_info->diffuse = {0.714f, 0.4284f, 0.18144f, 0.0f};
    bottom_plane_material_info->specular = {0.393548f, 0.271906f, 0.166721f};
    bottom_plane_material_info->shininess = 0.25f * 128.0f;

    auto far_plane_material_info = reinterpret_cast<Material_info*>(material_info_contents + 256 * 3);

    far_plane_material_info->ambient = {0.2125f, 0.1275f, 0.054f, 0.0f};
    far_plane_material_info->diffuse = {0.714f, 0.4284f, 0.18144f, 0.0f};
    far_plane_material_info->specular = {0.393548f, 0.271906f, 0.166721f};
    far_plane_material_info->shininess = 0.25f * 128.0f;

    auto cube_material_info = reinterpret_cast<Material_info*>(material_info_contents + 256 * 4);

    cube_material_info->ambient = {cfgs_.cube.ambient, 0.0};
    cube_material_info->diffuse = {cfgs_.cube.diffuse, 0.0};
    cube_material_info->specular = cfgs_.cube.specular;
    cube_material_info->shininess = cfgs_.cube.shininess;

    auto torus_material_info = reinterpret_cast<Material_info*>(material_info_contents + 256 * 5);

    torus_material_info->ambient = {cfgs_.torus.ambient, 0.0};
    torus_material_info->diffuse = {cfgs_.torus.diffuse, 0.0};
    torus_material_info->specular = cfgs_.torus.specular;
    torus_material_info->shininess = cfgs_.torus.shininess;

    auto sphere_material_info = reinterpret_cast<Material_info*>(material_info_contents + 256 * 6);

    sphere_material_info->ambient = {cfgs_.sphere.ambient, 0.0};
    sphere_material_info->diffuse = {cfgs_.sphere.diffuse, 0.0};
    sphere_material_info->specular = cfgs_.sphere.specular;
    sphere_material_info->shininess = cfgs_.sphere.shininess;

    buffers_["material_info"]->unmap();

    Render_encoder_desc render_encoder_desc;

    render_encoder_desc.colors[0].image = images_["light_color"].get();
    render_encoder_desc.colors[0].load_op = Load_op::clear;
    render_encoder_desc.colors[0].clear_value.r = 0.15f;
    render_encoder_desc.colors[0].clear_value.g = 0.15f;
    render_encoder_desc.colors[0].clear_value.b = 0.15f;
    render_encoder_desc.colors[0].clear_value.a = 1.0f;
    render_encoder_desc.depth_stencil.image = images_["light_depth_stencil"].get();
    render_encoder_desc.depth_stencil.load_op = Load_op::clear;
    render_encoder_desc.depth_stencil.clear_value.d = 1.0f;
    render_encoder_desc.depth_stencil.clear_value.s = 0;
    render_encoder_desc.depth_stencil.store_op = Store_op::dont_care;

    auto pipeline = [&](uint32_t style) {
        switch (style) {
            case 0:
                return pipelines_["flat"].get();
            case 1:
                return pipelines_["gouraud"].get();
            case 2:
                return pipelines_["phong"].get();
            default:
                throw runtime_error("invalid the light style");
        }
    };

    auto render_encoder = cmd_buffer_->create(render_encoder_desc);

    render_encoder->vertex_buffer(buffers_["cube_vertex"].get(), 0, 0);
    render_encoder->index_buffer(buffers_["cube_index"].get(), 0, Index_type::uint16);
    render_encoder->shader_buffer(buffers_["matrix_info"].get(), 0, 0);
    render_encoder->pipeline(pipelines_["lamp"].get());
    render_encoder->draw_indexed(draw_counts_["cube"]);

    render_encoder->vertex_buffer(buffers_["plane_vertex"].get(), 0, 0);
    render_encoder->index_buffer(buffers_["plane_index"].get(), 0, Index_type::uint16);
    render_encoder->shader_buffer(buffers_["matrix_info"].get(), 512 * 2, 0);
    render_encoder->shader_buffer(buffers_["light_info"].get(), 0, 1);
    render_encoder->shader_buffer(buffers_["material_info"].get(), 256 * 2, 2);
    render_encoder->pipeline(pipelines_["phong"].get());
    render_encoder->draw_indexed(draw_counts_["plane"]);

    render_encoder->shader_buffer(buffers_["matrix_info"].get(), 512 * 3, 0);
    render_encoder->shader_buffer(buffers_["light_info"].get(), 0, 1);
    render_encoder->shader_buffer(buffers_["material_info"].get(), 256 * 3, 2);
    render_encoder->draw_indexed(draw_counts_["plane"]);

    render_encoder->vertex_buffer(buffers_["cube_vertex"].get(), 0, 0);
    render_encoder->index_buffer(buffers_["cube_index"].get(), 0, Index_type::uint16);
    render_encoder->shader_buffer(buffers_["matrix_info"].get(), 512 * 4, 0);
    render_encoder->shader_buffer(buffers_["light_info"].get(), 0, 1);
    render_encoder->shader_buffer(buffers_["material_info"].get(), 256 * 4, 2);
    render_encoder->pipeline(pipeline(cfgs_.cube.style));
    render_encoder->draw_indexed(draw_counts_["cube"]);

    render_encoder->vertex_buffer(buffers_["torus_vertex"].get(), 0, 0);
    render_encoder->index_buffer(buffers_["torus_index"].get(), 0, Index_type::uint16);
    render_encoder->shader_buffer(buffers_["matrix_info"].get(), 512 * 5, 0);
    render_encoder->shader_buffer(buffers_["light_info"].get(), 0, 1);
    render_encoder->shader_buffer(buffers_["material_info"].get(), 256 * 5, 2);
    render_encoder->pipeline(pipeline(cfgs_.torus.style));
    render_encoder->draw_indexed(draw_counts_["torus"]);

    render_encoder->vertex_buffer(buffers_["sphere_vertex"].get(), 0, 0);
    render_encoder->index_buffer(buffers_["sphere_index"].get(), 0, Index_type::uint16);
    render_encoder->shader_buffer(buffers_["matrix_info"].get(), 512 * 6, 0);
    render_encoder->shader_buffer(buffers_["light_info"].get(), 0, 1);
    render_encoder->shader_buffer(buffers_["material_info"].get(), 256 * 6, 2);
    render_encoder->pipeline(pipeline(cfgs_.sphere.style));
    render_encoder->draw_indexed(draw_counts_["sphere"]);

    render_encoder->end();
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_demo::record_present_render_pass_()
{
    ImGui::NewFrame();
    ImGui::Begin("configs");

    if (ImGui::CollapsingHeader("camera")) {
        auto& cfgs = cfgs_.camera;

        ImGui::DragFloat3("camera translation", &cfgs.translation[0], 0.1f);
        ImGui::SliderFloat("camera fov", &cfgs.fov, 30.0f, 90.0f);
        ImGui::SliderFloat("camera aspect", &cfgs.aspect, 0.0f, 10.0f);
        ImGui::SliderFloat("camera near", &cfgs.near, 0.01f, 1.0f);
        ImGui::SliderFloat("camera far", &cfgs.far, 100.0f, 1000.0f);
    }

    if (ImGui::CollapsingHeader("light")) {
        auto& cfgs = cfgs_.light;

        ImGui::DragFloat3("light translation", &cfgs.translation[0], 0.1f);
        ImGui::ColorEdit3("light ambient", &cfgs.ambient[0]);
        ImGui::ColorEdit3("light diffuse", &cfgs.diffuse[0]);
        ImGui::ColorEdit3("light specular", &cfgs.specular[0]);
    }

    array<const char*, 3> items {"flat", "gouraud", "phong"};

    if (ImGui::CollapsingHeader("cube")) {
        auto& cfgs = cfgs_.cube;

        ImGui::Checkbox("cube animation", &cfgs.animation);
        ImGui::DragFloat3("cube translation", &cfgs.translation[0], 0.1f);
        ImGui::DragFloat3("cube scale", &cfgs.scale[0], 0.1f);
        ImGui::DragFloat3("cube rotation", &cfgs.rotation[0], 0.1f);
        ImGui::Combo("cube style", &cfgs.style, &items[0], items.size());
        ImGui::ColorEdit3("cube ambient", &cfgs.ambient[0]);
        ImGui::ColorEdit3("cube diffuse", &cfgs.diffuse[0]);
        ImGui::ColorEdit3("cube specular", &cfgs.specular[0]);
        ImGui::SliderFloat("cube shininess", &cfgs.shininess, 1.0f, 200.0f);
    }

    if (ImGui::CollapsingHeader("torus")) {
        auto& cfgs = cfgs_.torus;

        ImGui::Checkbox("torus animation", &cfgs.animation);
        ImGui::DragFloat3("torus translation", &cfgs.translation[0], 0.1f);
        ImGui::DragFloat3("torus scale", &cfgs.scale[0], 0.1f);
        ImGui::DragFloat3("torus rotation", &cfgs.rotation[0], 0.1f);
        ImGui::Combo("torus style", &cfgs.style, &items[0], items.size());
        ImGui::ColorEdit3("torus ambient", &cfgs.ambient[0]);
        ImGui::ColorEdit3("torus diffuse", &cfgs.diffuse[0]);
        ImGui::ColorEdit3("torus specular", &cfgs.specular[0]);
        ImGui::SliderFloat("torus shininess", &cfgs.shininess, 1.0f, 200.0f);
    }

    if (ImGui::CollapsingHeader("sphere")) {
        auto& cfgs = cfgs_.sphere;

        ImGui::Checkbox("sphere animation", &cfgs.animation);
        ImGui::DragFloat3("sphere translation", &cfgs.translation[0], 0.1f);
        ImGui::DragFloat3("sphere scale", &cfgs.scale[0], 0.1f);
        ImGui::DragFloat3("sphere rotation", &cfgs.rotation[0], 0.1f);
        ImGui::Combo("sphere style", &cfgs.style, &items[0], items.size());
        ImGui::ColorEdit3("sphere ambient", &cfgs.ambient[0]);
        ImGui::ColorEdit3("sphere diffuse", &cfgs.diffuse[0]);
        ImGui::ColorEdit3("sphere specular", &cfgs.specular[0]);
        ImGui::SliderFloat("sphere shininess", &cfgs.shininess, 1.0f, 200.0f);
    }

    ImGui::End();
    ImGui::Render();
    ImGui::EndFrame();

    auto draw_data = ImGui::GetDrawData();

    if (draw_data->CmdListsCount) {
        auto vertex_data {static_cast<ImDrawVert*>(buffers_["imgui_vertex"]->map())};
        auto index_data {static_cast<ImDrawIdx*>(buffers_["imgui_index"]->map())};

        for (auto i = 0; i != draw_data->CmdListsCount; ++i) {
            auto cmd_list = draw_data->CmdLists[i];

            memcpy(vertex_data, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
            vertex_data += cmd_list->VtxBuffer.Size;

            memcpy(index_data, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
            index_data += cmd_list->IdxBuffer.Size;
        }
    }

    Render_encoder_desc desc;

    desc.colors[0].image = swap_chain_->acquire();
    desc.colors[0].load_op = Load_op::dont_care;

    auto render_encoder = cmd_buffer_->create(desc);

    render_encoder->shader_texture(images_["light_color"].get(), samplers_["light_linear"].get(), 0);
    render_encoder->pipeline(pipelines_["composite"].get());
    render_encoder->draw(3, 0);

    if (draw_data->CmdListsCount) {
        render_encoder->shader_buffer(buffers_["imgui_shader_Imgui_info"].get(), 0, 0);
        render_encoder->shader_texture(images_["imgui_font"].get(), samplers_["light_linear"].get(), 0);
        render_encoder->pipeline(pipelines_["imgui"].get());

        auto vertex_buffer_offset {0};
        auto index_buffer_offset {0};

        for (auto i = 0; i != draw_data->CmdListsCount; ++i) {
            render_encoder->vertex_buffer(buffers_["imgui_vertex"].get(), vertex_buffer_offset, 0);
            render_encoder->index_buffer(buffers_["imgui_index"].get(), index_buffer_offset, Index_type::uint16);

            auto cmd_list = draw_data->CmdLists[i];

            for (auto& cmd : cmd_list->CmdBuffer)
                render_encoder->draw_indexed(cmd.ElemCount, cmd.IdxOffset);

            vertex_buffer_offset += cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
            index_buffer_offset += cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
        }
    }

    render_encoder->end();
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_demo::init_swap_chain_(Platform_lib::Window* window)
{
    try {
        // create a swap chain.
        Swap_chain_desc swap_chain_desc;

#if __APPLE__
        swap_chain_desc.image_format = Format::bgra8_unorm;
#else
        swap_chain_desc.image_format = Format::rgba8_unorm;
#endif
        swap_chain_desc.image_extent = window->extent();
        swap_chain_desc.window = window->window();

        swap_chain_ = device_->create(swap_chain_desc);
    }
    catch(exception& e) {
        throw runtime_error("fail to create a demo");
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_demo::init_cmd_buffer_()
{
    try {
        cmd_buffer_ = device_->create(Cmd_buffer_desc {});
    }
    catch(exception& e) {
        throw runtime_error("fail to create a demo");
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_demo::init_fence_()
{
    try {
        fence_ = device_->create(Fence_desc { true });
    }
    catch(exception& e) {
        throw runtime_error("fail to create a demo");
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_demo::init_imgui_()
{
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    auto& io = ImGui::GetIO();

    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    io.DisplaySize.x = swap_chain_->image_extent().w;
    io.DisplaySize.y = swap_chain_->image_extent().h;
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_demo::init_imgui_resources_()
{
    try {
        Buffer_desc desc;

        desc.size = 5 * 1024 * 1024;

        buffers_["imgui_vertex"] = device_->create(desc);
    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    try {
        Buffer_desc desc;

        desc.size = 1024 * 1024;

        buffers_["imgui_index"] = device_->create(desc);
    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    try {
        auto& io = ImGui::GetIO();

        Imgui_info imgui_info {
            {-1.0f, 1.0f},
            {2.0f / io.DisplaySize.x, 2.0f / -io.DisplaySize.y}
        };

        Buffer_desc desc;

        desc.size = sizeof(Imgui_info);
        desc.data = &imgui_info;

        buffers_["imgui_shader_Imgui_info"] = device_->create(desc);
    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    try {
        uint8_t* pixels;
        int32_t width, height;
        auto& imgui_io = ImGui::GetIO();

        imgui_io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        Buffer_desc buffer_desc;

        buffer_desc.size = width * height * 4;
        buffer_desc.data = pixels;

        auto staging_buffer = device_->create(buffer_desc);

        Image_desc image_desc;

        image_desc.format = Format::rgba8_unorm;
        image_desc.extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1};

        images_["imgui_font"] = device_->create(image_desc);

        auto cmd_buffer = device_->create(Cmd_buffer_desc {});
        auto blit_encoder = cmd_buffer->create(Blit_encoder_desc {});

        Buffer_image_copy_region copy_region;

        copy_region.buffer_row_size = width * 4;
        copy_region.buffer_image_height = height;
        copy_region.image_extent = image_desc.extent;

        blit_encoder->copy(staging_buffer.get(), images_["imgui_font"].get(), copy_region);
        blit_encoder->end();
        cmd_buffer->end();
        device_->submit(cmd_buffer.get());
        device_->wait_idle();
    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    try {
        // create shaders.
        const vector<string> pathes {
#if defined(__ANDROID__)
            "/sdcard/Android/data/com.ff.gfx_demo/files/composite.vert",
            "/sdcard/Android/data/com.ff.gfx_demo/files/composite.frag"
#else
            "../../../gfx/res/shader/composite.vert",
            "../../../gfx/res/shader/composite.frag"
#endif
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

        pipelines_["composite"] = device_->create(pipeline_desc);
    }
    catch (exception& e) {
        throw runtime_error("fail to create gfx demo");
    }

    try {
        // create shaders.
        const vector<string> pathes {
#if defined(__ANDROID__)
            "/sdcard/Android/data/com.ff.gfx_demo/files/imgui.vert",
            "/sdcard/Android/data/com.ff.gfx_demo/files/imgui.frag"
#else
            "../../../gfx/res/shader/imgui.vert",
            "../../../gfx/res/shader/imgui.frag"
#endif
        };

        array<unique_ptr<Shader>, 2> shaders;

        for (auto i = 0; i != 2; ++i) {
            Shader_desc shader_desc;

            shader_desc.type = static_cast<Shader_type>(i);
            shader_desc.src = compiler_.compile(pathes[i]);

            shaders[i] = device_->create(shader_desc);
        }

        Vertex_input vertex_input;

        vertex_input.attributes[0].binding = 0;
        vertex_input.attributes[0].format = Format::rg32_float;
        vertex_input.attributes[0].offset = IM_OFFSETOF(ImDrawVert, pos);
        vertex_input.attributes[1].binding = 0;
        vertex_input.attributes[1].format = Format::rgba8_unorm;
        vertex_input.attributes[1].offset = IM_OFFSETOF(ImDrawVert, col);
        vertex_input.attributes[2].binding = 0;
        vertex_input.attributes[2].format = Format::rg32_float;
        vertex_input.attributes[2].offset = IM_OFFSETOF(ImDrawVert, uv);
        vertex_input.bindings[0].stride = sizeof(ImDrawVert);

        // create a pipeline.
        Pipeline_desc pipeline_desc;

        pipeline_desc.vertex_input = vertex_input;
        pipeline_desc.vertex_shader = shaders[0].get();
        pipeline_desc.rasterization.front_face = Front_face::clockwise;
        pipeline_desc.fragment_shader = shaders[1].get();
        pipeline_desc.color_blend.attachments[0].blend = true;
        pipeline_desc.color_blend.attachments[0].src_rgb_blend_factor = Blend_factor::src_alpha;
        pipeline_desc.color_blend.attachments[0].dst_rgb_blend_factor = Blend_factor::one_minus_src_alpha;
        pipeline_desc.color_blend.attachments[0].src_a_blend_factor = Blend_factor::src_alpha;
        pipeline_desc.color_blend.attachments[0].dst_a_blend_factor = Blend_factor::one_minus_src_alpha;
        pipeline_desc.output_merger.color_formats[0] = swap_chain_->image_format();

        pipelines_["imgui"] = device_->create(pipeline_desc);
    }
    catch( exception& e) {
        throw runtime_error("fail to create gfx demo");
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Gfx_demo::fini_imgui_()
{
    ImGui::DestroyContext();
}

//----------------------------------------------------------------------------------------------------------------------
