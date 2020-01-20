//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_DEMO_PHONG_DEMO_GUARD
#define GFX_DEMO_PHONG_DEMO_GUARD

#include "Demo.h"
#include "util.h"

//----------------------------------------------------------------------------------------------------------------------

class Phong_demo : public Demo {
public:
    Phong_demo();

    void render() override;

private:
    void init_depth_stencil_image_();

    void init_vertex_buffer_();

    void init_index_buffer_();

    void init_shaders_();

    void init_pipeline_();

    void init_uniform_buffers_();

private:
    Torus torus_;
    std::unique_ptr<Gfx_lib::Image> depth_stencil_image_;
    std::unique_ptr<Gfx_lib::Buffer> vertex_buffer_;
    Gfx_lib::Vertex_input vertex_input_;
    std::unique_ptr<Gfx_lib::Buffer> index_buffer_;
    uint32_t index_count_;
    std::array<std::unique_ptr<Gfx_lib::Shader>, 2> shaders_;
    std::unique_ptr<Gfx_lib::Pipeline> pipeline_;
    std::unordered_map<std::string, std::unique_ptr<Gfx_lib::Buffer>> uniform_buffers_;
    float angle_;
};

//----------------------------------------------------------------------------------------------------------------------

#endif // GFX_DEMO_PHONG_DEMO_GUARD
