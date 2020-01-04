//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_DEMO_TRIANGLE_DEMO_GUARD
#define GFX_DEMO_TRIANGLE_DEMO_GUARD

#include "Demo.h"

//----------------------------------------------------------------------------------------------------------------------

class Triangle_demo : public Demo {
public:
    Triangle_demo();

    void render() override;

private:
    void init_pipeline_();

private:
     std::unique_ptr<Gfx_lib::Pipeline> pipeline_;
};

//----------------------------------------------------------------------------------------------------------------------

#endif // GFX_DEMO_TRIANGLE_DEMO_GUARD
