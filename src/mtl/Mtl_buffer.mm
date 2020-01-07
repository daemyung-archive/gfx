//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "std_lib.h"
#include "mtl_lib.h"
#include "Mtl_buffer.h"
#include "Mtl_device.h"

using namespace std;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Mtl_buffer::Mtl_buffer(const Buffer_desc& desc, Mtl_device* device) :
    Buffer(),
    device_ { device },
    type_ { desc.type },
    buffer_ { nil }
{
    init_buffer_(desc);
}

//----------------------------------------------------------------------------------------------------------------------

void* Mtl_buffer::map()
{
    return [buffer_ contents];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_buffer::unmap()
{
}

//----------------------------------------------------------------------------------------------------------------------

Device* Mtl_buffer::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

Heap_type Mtl_buffer::type() const
{
    return type_;
}

//----------------------------------------------------------------------------------------------------------------------

uint64_t Mtl_buffer::size() const
{
    return [buffer_ length];
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_buffer::init_buffer_(const Buffer_desc& desc)
{
    const auto options = convert<MTLResourceOptions>(desc.type);

    if (desc.data) {
        buffer_ = [device_->device() newBufferWithBytes:desc.data
                                                 length:desc.size
                                                options:options];
    }
    else {
        buffer_ = [device_->device() newBufferWithLength:desc.size
                                                 options:options];
    }
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
