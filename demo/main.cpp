//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include <cstdint>
#include <string>
#include <cxxopts.hpp>
#include <platform/Window.h>
#include "Gfx_demo.h"

using namespace std;
using namespace cxxopts;
using namespace Platform_lib;

//----------------------------------------------------------------------------------------------------------------------

unique_ptr<Window> window_;
unique_ptr<Gfx_demo> gfx_demo_;

//----------------------------------------------------------------------------------------------------------------------

void on_startup()
{
    gfx_demo_->connect(window_.get());
}

//----------------------------------------------------------------------------------------------------------------------

void on_shutdown()
{
    gfx_demo_ = nullptr;
}

//----------------------------------------------------------------------------------------------------------------------

void on_render()
{
    gfx_demo_->render();
}

//----------------------------------------------------------------------------------------------------------------------

void on_touch_down()
{
    gfx_demo_->touch_down();
}

//----------------------------------------------------------------------------------------------------------------------

void on_touch_move(float x, float y)
{
    gfx_demo_->touch_move(x, y);
}

//----------------------------------------------------------------------------------------------------------------------

void on_touch_up()
{
    gfx_demo_->touch_up();
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
    window_->touch_down_signal.connect(&on_touch_down);
    window_->touch_move_signal.connect(&on_touch_move);
    window_->touch_up_signal.connect(&on_touch_up);

    gfx_demo_ = make_unique<Gfx_demo>();

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
    window_->touch_down_signal.connect(&on_touch_down);
    window_->touch_move_signal.connect(&on_touch_move);
    window_->touch_up_signal.connect(&on_touch_up);

    gfx_demo_ = make_unique<Gfx_demo>();

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
