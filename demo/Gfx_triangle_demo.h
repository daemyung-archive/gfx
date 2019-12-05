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
    Gfx_triangle_demo(uint32_t w, uint32_t h);

    void run();

private:
    void init_window_(uint32_t w, uint32_t h);

    void init_resources_();

    void on_startup_();

    void on_shutdown_();

    void on_render_();

private:
    std::unique_ptr<Window> window_;
    Spirv_compiler compiler_;
    std::unique_ptr<Device> device_;
    std::unique_ptr<Swap_chain> swap_chain_;
    uint32_t frame_index_;
    std::array<std::unique_ptr<Cmd_buffer>, 3> cmd_buffers_;
    std::array<std::unique_ptr<Fence>, 3> fences_;
    std::unique_ptr<Buffer> vertex_buffer_;
    std::unique_ptr<Pipeline> render_pipeline_;
};

//----------------------------------------------------------------------------------------------------------------------
