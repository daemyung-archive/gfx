//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_DEVICE_GUARD
#define GFX_DEVICE_GUARD

#include <memory>
#include "enums.h"
#include "Buffer.h"
#include "Image.h"
#include "Sampler.h"
#include "Shader.h"
#include "Pipeline.h"
#include "Swap_chain.h"
#include "Cmd_buffer.h"
#include "Fence.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

struct Caps final {
    Coords window_coords {Coords::invalid};
    Coords texture_coords {Coords::invalid};
};

//----------------------------------------------------------------------------------------------------------------------

class Device {
public:
    static std::unique_ptr<Device> create();

    virtual ~Device() = default;

    virtual std::unique_ptr<Buffer> create(const Buffer_desc& desc) = 0;

    virtual std::unique_ptr<Image> create(const Image_desc& desc) = 0;

    virtual std::unique_ptr<Sampler> create(const Sampler_desc& desc) = 0;

    virtual std::unique_ptr<Shader> create(const Shader_desc& desc) = 0;

    virtual std::unique_ptr<Pipeline> create(const Pipeline_desc& desc) = 0;

    virtual std::unique_ptr<Swap_chain> create(const Swap_chain_desc& desc) = 0;

    virtual std::unique_ptr<Cmd_buffer> create(const Cmd_buffer_desc& desc) = 0;

    virtual std::unique_ptr<Fence> create(const Fence_desc& desc) = 0;

    virtual void submit(Cmd_buffer* cmd_buffer, Fence* fence = nullptr) = 0;

    virtual void wait_idle() = 0;

    inline auto caps() const noexcept
    { return caps_; }

protected:
    Caps caps_;
};

//----------------------------------------------------------------------------------------------------------------------
    
} // of namespace Gfx_lib

#endif // GFX_DEVICE_GUARD
