//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include <cstdint>
#include <string>
#include <cxxopts.hpp>
#include "Gfx_triangle_demo.h"
#include "Gfx_texture_demo.h"

using namespace std;
using namespace cxxopts;

//----------------------------------------------------------------------------------------------------------------------

template<typename T>
void run(uint32_t w, uint32_t h)
{
    T(w, h).run();
}

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    Options options("gfx_demo", "Various gfx demos");

    options.add_options()
        ("w, width", "window width", value<uint32_t>()->default_value("360"))
        ("h, height", "window height", value<uint32_t>()->default_value("640"))
        ("t, target", "demo name", value<string>()->default_value("triangle"));

    auto result = options.parse(argc, argv);
    auto target = result["target"].as<string>();

    if ("triangle" == target)
        run<Gfx_triangle_demo>(result["w"].as<uint32_t>(), result["h"].as<uint32_t>());
    else if("texture" == target)
        run<Gfx_texture_demo>(result["w"].as<uint32_t>(), result["h"].as<uint32_t>());

    return 0;
}

//----------------------------------------------------------------------------------------------------------------------
