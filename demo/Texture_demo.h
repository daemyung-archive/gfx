//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_DEMO_TEXTURE_DEMO_GUARD
#define GFX_DEMO_TEXTURE_DEMO_GUARD

#include "Demo.h"

//----------------------------------------------------------------------------------------------------------------------

class Texture_demo : public Demo
{
public:
    Texture_demo();

    void render() override;

private:
    void init_vertex_buffer_();

    void init_image_();

    void init_sampler_();

    void init_pipeline_();

private:
    std::unique_ptr<Gfx_lib::Buffer> vertex_buffer_;
    std::unique_ptr<Gfx_lib::Image> image_;
    std::unique_ptr<Gfx_lib::Sampler> sampler_;
    std::unique_ptr<Gfx_lib::Pipeline> pipeline_;
};

//----------------------------------------------------------------------------------------------------------------------

#endif // GFX_DEMO_TEXTURE_DEMO_GUARD
