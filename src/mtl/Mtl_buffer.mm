//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "mtl/Mtl_buffer.h"
#include "mtl/Mtl_device.h"
#include "mtl/mtl_lib_modules.h"
#include "std_lib_modules.h"

#include <vector>

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

gsl::span<std::byte> Mtl_buffer::map()
{
    return { static_cast<byte*>([buffer_ contents]), static_cast<ptrdiff_t>([buffer_ allocatedSize]) };
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_buffer::unmap()
{
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
    const auto options = convert(desc.type);

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
