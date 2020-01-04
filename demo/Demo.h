//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_DEMO_DEMO_GUARD
#define GFX_DEMO_DEMO_GUARD

#include <memory>
#include <vector>
#include <platform/Window.h>
#include <sc/Spirv_compiler.h>
#include <gfx/Device.h>

//----------------------------------------------------------------------------------------------------------------------

class Demo {
public:
    Demo();

    virtual ~Demo();

    void connect(Platform_lib::Window* window);

    virtual void render() = 0;

private:
    void init_device_();

    void init_swap_chain_(Platform_lib::Window* window);

    void init_cmd_buffer_();

    void init_fence_();

protected:
    Sc_lib::Spirv_compiler compiler_;
    std::unique_ptr<Gfx_lib::Device> device_;
    std::unique_ptr<Gfx_lib::Swap_chain> swap_chain_;
    std::unique_ptr<Gfx_lib::Cmd_buffer> cmd_buffer_;
    std::unique_ptr<Gfx_lib::Fence> fence_;
};

//----------------------------------------------------------------------------------------------------------------------

#endif // GFX_DEMO_DEMO_GUARD
