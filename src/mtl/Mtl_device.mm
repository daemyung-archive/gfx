//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "mtl_lib_modules.h"
#include "std_lib_modules.h"
#include "Mtl_device.h"
#include "Mtl_buffer.h"
#include "Mtl_image.h"
#include "Mtl_sampler.h"
#include "Mtl_shader.h"
#include "Mtl_pipeline.h"
#include "Mtl_swap_chain.h"
#include "Mtl_cmd_list.h"
#include "Mtl_fence.h"

using namespace std;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Mtl_device::Mtl_device() :
    device_ { nil },
    command_queue_ { nil },
    used_command_buffers_ { [NSMutableSet new] },
    queue_mutex_ {}
{
    init_device_();
    init_command_queue_();
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Buffer> Mtl_device::make(const Buffer_desc& desc)
{
    return make_unique<Mtl_buffer>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Image> Mtl_device::make(const Image_desc& desc)
{
    return make_unique<Mtl_image>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Sampler> Mtl_device::make(const Sampler_desc& desc)
{
    return make_unique<Mtl_sampler>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Shader> Mtl_device::make(const Shader_desc& desc)
{
    return make_unique<Mtl_shader>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Pipeline> Mtl_device::make(const Pipeline_desc<Pipeline_type::render>& desc)
{
    return make_unique<Mtl_pipeline>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Swap_chain> Mtl_device::make(const Swap_chain_desc& desc)
{
    return make_unique<Mtl_swap_chain>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Cmd_list> Mtl_device::make_cmd_list()
{
    return make_unique<Mtl_cmd_list>(this);
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Fence> Mtl_device::make(const Fence_desc& desc)
{
    return make_unique<Mtl_fence>(desc, this);
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_device::submit(Cmd_list* cmd_list, Fence* fence)
{
    auto mtl_cmd_list = static_cast<Mtl_cmd_list*>(cmd_list);
    __block auto semaphore = fence ? static_cast<Mtl_fence*>(fence)->semaphore() : nil;

    [mtl_cmd_list->command_buffer() addCompletedHandler:^(id<MTLCommandBuffer> command_buffer) {
        if (queue_mutex_.try_lock()) {
            [used_command_buffers_ removeObject:command_buffer];
            queue_mutex_.unlock();
        }

        if (semaphore)
            dispatch_semaphore_signal(semaphore);
    }];
    [mtl_cmd_list->command_buffer() commit];

    lock_guard<mutex> lock { queue_mutex_ };

    [used_command_buffers_ addObject:mtl_cmd_list->command_buffer()];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_device::wait_idle()
{
    lock_guard<mutex> lock { queue_mutex_ };

    for (id<MTLCommandBuffer> command_buffer in used_command_buffers_)
        [command_buffer waitUntilCompleted];

    [used_command_buffers_ removeAllObjects];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_device::init_device_()
{
    device_ = MTLCreateSystemDefaultDevice();

    if (!device_)
        throw runtime_error("fail to create device.");
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_device::init_command_queue_()
{
    command_queue_ = [device_ newCommandQueue];

    if (!command_queue_)
        throw runtime_error("fail to create device.");
}

//----------------------------------------------------------------------------------------------------------------------
    
} // of namespace Gfx_lib
