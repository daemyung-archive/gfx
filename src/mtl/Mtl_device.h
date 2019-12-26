//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_MTL_DEVICE_GUARD
#define GFX_MTL_DEVICE_GUARD

#include <mutex>
#include <Foundation/Foundation.h>
#include <Metal/Metal.h>
#include "Device.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Mtl_device final : public Device {
public:
    Mtl_device();

    std::unique_ptr<Buffer> make(const Buffer_desc& desc) override;

    std::unique_ptr<Image> make(const Image_desc& desc) override;

    std::unique_ptr<Sampler> make(const Sampler_desc& desc) override;

    std::unique_ptr<Shader> make(const Shader_desc& desc) override;

    std::unique_ptr<Pipeline> make(const Pipeline_desc& desc) override;

    std::unique_ptr<Swap_chain> make(const Swap_chain_desc& desc) override;

    std::unique_ptr<Cmd_buffer> make_cmd_buffer() override;

    std::unique_ptr<Fence> make(const Fence_desc& desc) override;

    void submit(Cmd_buffer* cmd_buffer, Fence* fence = nullptr) override;

    void wait_idle() override;

    inline auto device() const noexcept
    { return device_; }

    inline auto command_queue() const noexcept
    { return command_queue_; }

private:
    void init_device_();

    void init_command_queue_();

private:
    id<MTLDevice> device_;
    id<MTLCommandQueue> command_queue_;
    NSMutableSet<id<MTLCommandBuffer>>* used_command_buffers_;
    std::mutex queue_mutex_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_MTL_DEVICE_GUARD
