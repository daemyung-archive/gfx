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
    Buffer {desc},
    device_ {device},
    buffer_ {nil}
{
    init_buffer_(desc.data);
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

void Mtl_buffer::init_buffer_(const void* data)
{
    const auto options = to_MTLResourceOptions(heap_type_);

    if (data) {
        buffer_ = [device_->device() newBufferWithBytes:data
                                                 length:size_
                                                options:options];
    }
    else {
        buffer_ = [device_->device() newBufferWithLength:size_
                                                 options:options];
    }
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
