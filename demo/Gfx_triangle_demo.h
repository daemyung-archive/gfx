//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "platform_lib_modules.h"
#include "sc_lib_modules.h"
#include "gfx_lib_modules.h"

//----------------------------------------------------------------------------------------------------------------------

class Gfx_triangle_demo
{
public:
    Gfx_triangle_demo(Window* window);

    void run();

private:
    void init_window_(Window* window);

    void init_resources_();

    void on_startup_();

    void on_shutdown_();

    void on_render_();

private:
    template<typename T>
    using unique_ptr = std::unique_ptr<T>;
    template<typename T, size_t Size>
    using array = std::array<T, Size>;

    Window* window_;
    Spirv_compiler compiler_;
    unique_ptr<Device> device_;
    unique_ptr<Swap_chain> swap_chain_;
    uint32_t frame_index_;
    array<unique_ptr<Cmd_buffer>, 3> cmd_buffers_;
    array<unique_ptr<Fence>, 3> fences_;
    unique_ptr<Buffer> vertex_buffer_;
    unique_ptr<Pipeline> render_pipeline_;
};

//----------------------------------------------------------------------------------------------------------------------
