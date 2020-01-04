//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "Demo.h"

using namespace std;
using namespace Gfx_lib;

namespace {

// constexpr auto

} // of namespace

//----------------------------------------------------------------------------------------------------------------------

Demo::Demo() :
    compiler_ {},
    device_ {},
    swap_chain_ {},
    cmd_buffer_ {},
    fence_ {}
{
    init_device_();
}

//----------------------------------------------------------------------------------------------------------------------

Demo::~Demo()
{
    device_->wait_idle();
}

//----------------------------------------------------------------------------------------------------------------------

void Demo::connect(Platform_lib::Window* window)
{
    init_swap_chain_(window);
    init_cmd_buffer_();
    init_fence_();
}

//----------------------------------------------------------------------------------------------------------------------

void Demo::init_device_()
{
    try {
        device_ = Device::create();
    }
    catch(exception& e) {
        throw runtime_error("fail to create a demo");
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Demo::init_swap_chain_(Platform_lib::Window* window)
{
    try {
        // create a swap chain.
        Swap_chain_desc swap_chain_desc;

#if __APPLE__
        swap_chain_desc.image_format = Format::bgra8_unorm;
#else
        swap_chain_desc.image_format = Format::rgba8_unorm;
#endif
        swap_chain_desc.image_extent = window->extent();
        swap_chain_desc.window = window->window();

        swap_chain_ = device_->create(swap_chain_desc);
    }
    catch(exception& e) {
        throw runtime_error("fail to create a demo");
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Demo::init_cmd_buffer_()
{
    try {
        cmd_buffer_ = device_->create(Cmd_buffer_desc {});
    }
    catch(exception& e) {
        throw runtime_error("fail to create a demo");
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Demo::init_fence_()
{
    try {
        fence_ = device_->create(Fence_desc { true });
    }
    catch(exception& e) {
        throw runtime_error("fail to create a demo");
    }
}

//----------------------------------------------------------------------------------------------------------------------
