//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_DEMO_GUARD
#define GFX_DEMO_GUARD

#include <unordered_map>
#include <glm/glm.hpp>
#include <platform/Window.h>
#include <sc/Spirv_compiler.h>
#include <gfx/Device.h>

//----------------------------------------------------------------------------------------------------------------------

struct Camera_cfgs {
    int32_t projection {0};
    glm::vec3 translation {4.5f, 8.0f, 10.0f};
    float fov {45.0f};
    float aspect {1.778f};
    float near {0.01f};
    float far {100.0f};
};

//----------------------------------------------------------------------------------------------------------------------

struct Light_cfgs {
    glm::vec3 translation {-1.0f, 1.0f, 1.7f};
    glm::vec3 ambient {0.4f, 0.4f, 0.4f};
    glm::vec3 diffuse {1.0f, 1.0f, 1.0f};
    glm::vec3 specular {1.0f, 1.0f, 1.0f};
};

//----------------------------------------------------------------------------------------------------------------------

struct Primitive_cfgs {
    bool animation {true};
    glm::vec3 translation {0.0f, 0.0f, 0.0f};
    glm::vec3 scale {1.0f, 1.0f, 1.0f};
    glm::vec3 rotation {0.0f, 0.0f, 0.0f};
    int32_t style {0};
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess {100.0f};
};

//----------------------------------------------------------------------------------------------------------------------

struct Cfgs {
    Camera_cfgs camera;
    Light_cfgs light;
    Primitive_cfgs cube;
    Primitive_cfgs torus;
    Primitive_cfgs sphere;
};

//----------------------------------------------------------------------------------------------------------------------

class Gfx_demo final {
public:
    Gfx_demo();

    ~Gfx_demo();

    void connect(Platform_lib::Window* window);

    void render();

    void touch_down();

    void touch_move(float x, float y);

    void touch_up();

private:
    void init_cfgs_();

    void init_device_();

    void init_light_resources_();

    void init_swap_chain_(Platform_lib::Window* window);

    void init_cmd_buffer_();

    void init_fence_();

    void init_imgui_();

    void init_imgui_resources_();

    void fini_imgui_();

    void record_light_render_pass_();

    void record_present_render_pass_();

private:
    Cfgs cfgs_;
    Sc_lib::Spirv_compiler compiler_;
    std::unique_ptr<Gfx_lib::Device> device_;
    std::unordered_map<std::string, std::unique_ptr<Gfx_lib::Buffer>> buffers_;
    std::unordered_map<std::string, uint32_t> draw_counts_;
    std::unordered_map<std::string, std::unique_ptr<Gfx_lib::Image>> images_;
    std::unordered_map<std::string, std::unique_ptr<Gfx_lib::Sampler>> samplers_;
    std::unordered_map<std::string, std::unique_ptr<Gfx_lib::Pipeline>> pipelines_;
    std::unique_ptr<Gfx_lib::Swap_chain> swap_chain_;
    std::unique_ptr<Gfx_lib::Cmd_buffer> cmd_buffer_;
    std::unique_ptr<Gfx_lib::Fence> fence_;
};

//----------------------------------------------------------------------------------------------------------------------

#endif // GFX_DEMO_GUARD
