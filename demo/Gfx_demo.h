//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_DEMO_TRIANGLE_DEMO_GUARD
#define GFX_DEMO_TRIANGLE_DEMO_GUARD

#include "Demo.h"

//----------------------------------------------------------------------------------------------------------------------

class Gfx_demo : public Demo {
public:
    Gfx_demo();

    void render() override;

private:
    void init_images_();

    void init_pipeline_();

private:
    std::unique_ptr<Gfx_lib::Image> color_image_;
    std::unique_ptr<Gfx_lib::Image> depth_stencil_image_;
    std::unique_ptr<Gfx_lib::Pipeline> pipeline_;
};

//----------------------------------------------------------------------------------------------------------------------

#endif // GFX_DEMO_TRIANGLE_DEMO_GUARD
