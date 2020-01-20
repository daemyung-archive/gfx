//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include <cstdint>
#include <string>
#include <cxxopts.hpp>
#include <platform/Window.h>
#include "Triangle_demo.h"
#include "Texture_demo.h"
#include "Phong_demo.h"

using namespace std;
using namespace cxxopts;
using namespace Platform_lib;

//----------------------------------------------------------------------------------------------------------------------

unique_ptr<Window> window_;
unique_ptr<Demo> demo_;

//----------------------------------------------------------------------------------------------------------------------

void on_startup()
{
    demo_->connect(window_.get());
}

//----------------------------------------------------------------------------------------------------------------------

void on_shutdown()
{
    demo_ = nullptr;
}

//----------------------------------------------------------------------------------------------------------------------

void on_render()
{
    demo_->render();
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

    window_ = make_unique<Window>(window_desc);

    window_->startup_signal.connect(&on_startup);
    window_->shutdown_signal.connect(&on_shutdown);
    window_->render_signal.connect(&on_render);

    if ("triangle" == target)
        demo_ = make_unique<Triangle_demo>();
    else if ("texture" == target)
        demo_ = make_unique<Texture_demo>();
    else if ("phong" == target)
        demo_ = make_unique<Phong_demo>();

    window_->run();

    return 0;
}
#elif defined(__ANDROID__)
void android_main(struct android_app* state)
{
    Window_desc window_desc;

    window_desc.title = L"GFX Demo";
    window_desc.extent = { 360, 640 };
    window_desc.app = state;

    window_ = make_unique<Window>(window_desc);

    window_->startup_signal.connect(&on_startup);
    window_->shutdown_signal.connect(&on_shutdown);
    window_->render_signal.connect(&on_render);

    demo_ = make_unique<Triangle_demo>();

    window_->run();
}
#elif defined(_WIN32)
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    Window_desc window_desc;

    window_desc.title = L"GFX Demo";
    window_desc.extent = { 360, 640 };

    window_ = make_unique<Window>(window_desc);

    window_->startup_signal.connect(&on_startup);
    window_->shutdown_signal.connect(&on_shutdown);
    window_->render_signal.connect(&on_render);

    demo_ = make_unique<Triangle_demo>();

    window_->run();

    return 0;
}
#endif

//----------------------------------------------------------------------------------------------------------------------
