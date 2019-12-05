//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_DEVICE_GUARD
#define GFX_DEVICE_GUARD

#include <memory>
#include "gfx/enums.h"
#include "gfx/Buffer.h"
#include "gfx/Image.h"
#include "gfx/Sampler.h"
#include "gfx/Shader.h"
#include "gfx/Pipeline.h"
#include "gfx/Swap_chain.h"
#include "gfx/Cmd_buffer.h"
#include "gfx/Fence.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Device {
public:
    static std::unique_ptr<Device> make();

    virtual ~Device() = default;

    virtual std::unique_ptr<Buffer> make(const Buffer_desc& desc) = 0;

    virtual std::unique_ptr<Image> make(const Image_desc& desc) = 0;

    virtual std::unique_ptr<Sampler> make(const Sampler_desc& desc) = 0;

    virtual std::unique_ptr<Shader> make(const Shader_desc& desc) = 0;

    virtual std::unique_ptr<Pipeline> make(const Pipeline_desc<Pipeline_type::render>& desc) = 0;

    virtual std::unique_ptr<Swap_chain> make(const Swap_chain_desc& desc) = 0;

    virtual std::unique_ptr<Cmd_buffer> make_cmd_buffer() = 0;

    virtual std::unique_ptr<Fence> make(const Fence_desc& desc) = 0;

    virtual void submit(Cmd_buffer* cmd_buffer, Fence* fence = nullptr) = 0;

    virtual void wait_idle() = 0;
};

//----------------------------------------------------------------------------------------------------------------------
    
} // of namespace Gfx_lib

#endif // GFX_DEVICE_GUARD
