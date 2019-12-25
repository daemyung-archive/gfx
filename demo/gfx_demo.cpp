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
void run(Window* window)
{
    T(window).run();
}

//----------------------------------------------------------------------------------------------------------------------

#if TARGET_OS_IOS || TARGET_OS_OSX
int main(int argc, char* argv[])
{
    Options options("gfx_demo", "Various gfx demos");

    options.add_options()
        ("w, width", "window width", value<uint32_t>()->default_value("360"))
        ("h, height", "window height", value<uint32_t>()->default_value("640"))
        ("t, target", "demo name", value<string>()->default_value("triangle"));

    auto result = options.parse(argc, argv);
    auto target = result["target"].as<string>();

    Window_desc window_desc;

    window_desc.title = L"GFX Demo";
    window_desc.extent = { result["w"].as<uint32_t>(), result["h"].as<uint32_t>() };

    auto window = make_unique<Window>(window_desc);

    // run<Gfx_texture_demo>(window.get());

    if ("triangle" == target)
        run<Gfx_triangle_demo>(window.get());
    else if("texture" == target)
        run<Gfx_texture_demo>(window.get());

    return 0;
}
#elif defined(__ANDROID__)
void android_main(struct android_app* state)
{
    Window_desc window_desc;

    window_desc.title = L"GFX Demo";
    window_desc.extent = { 360, 640 };
    window_desc.app = state;

    auto window = make_unique<Window>(window_desc);

    run<Gfx_triangle_demo>(window.get());
}
#elif defined(_WIN32)
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    Window_desc window_desc;

    window_desc.title = L"GFX Demo";
    window_desc.extent = { 360, 640 };

    auto window = make_unique<Window>(window_desc);

    run<Gfx_triangle_demo>(window.get());

    return 0;
}
#endif

//----------------------------------------------------------------------------------------------------------------------
