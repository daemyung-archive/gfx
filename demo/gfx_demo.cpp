//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "Gfx_triangle_demo.h"
#include "Gfx_texture_demo.h"

//----------------------------------------------------------------------------------------------------------------------

template<typename T>
void run(uint32_t w, uint32_t h)
{
    T(w, h).run();
}

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    // run<Gfx_triangle_demo>(360, 640);
    run<Gfx_texture_demo>(360, 640);
}

//----------------------------------------------------------------------------------------------------------------------
