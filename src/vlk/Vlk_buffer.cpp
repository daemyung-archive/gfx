//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "std_lib.h"
#include "vlk_lib.h"
#include "Vlk_buffer.h"
#include "Vlk_device.h"

using namespace std;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Vlk_buffer::Vlk_buffer(const Buffer_desc& desc, Vlk_device* device) :
    Buffer {desc},
    device_ {device},
    buffer_ {VK_NULL_HANDLE},
    alloc_ {VK_NULL_HANDLE},
    contents_ {nullptr}
{
    init_buffer_and_alloc_(desc.data);
}

//----------------------------------------------------------------------------------------------------------------------

Vlk_buffer::~Vlk_buffer()
{
    fini_buffer_and_alloc_();
}

//----------------------------------------------------------------------------------------------------------------------

void* Vlk_buffer::map()
{
    return contents_;
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_buffer::unmap()
{
    vmaFlushAllocation(device_->allocator(), alloc_, 0, VK_WHOLE_SIZE);
}

//----------------------------------------------------------------------------------------------------------------------

Device* Vlk_buffer::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_buffer::init_buffer_and_alloc_(const void* data)
{
    // configure the required buffer usage.
    constexpr auto usage {
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
        VK_BUFFER_USAGE_TRANSFER_DST_BIT
    };

    // configure a buffer create info.
    VkBufferCreateInfo create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    create_info.size = size_;
    create_info.usage = usage;

    // configure an allocation create info.
    VmaAllocationCreateInfo alloc_create_info {};

    alloc_create_info.usage = to_VmaMemoryUsage(heap_type_);

    if (Heap_type::local != heap_type_)
        alloc_create_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    // try to create a buffer and an allocation.
    if (vmaCreateBuffer(device_->allocator(), &create_info, &alloc_create_info, &buffer_, &alloc_, nullptr))
        throw runtime_error("fail to create buffer");

    if (Heap_type::local != heap_type_)
        vmaMapMemory(device_->allocator(), alloc_, &contents_);

    if (data) {
        memcpy(contents_, data, size_);
        vmaFlushAllocation(device_->allocator(), alloc_, 0, VK_WHOLE_SIZE);
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_buffer::fini_buffer_and_alloc_()
{
    if (Heap_type::local != heap_type_)
        vmaUnmapMemory(device_->allocator(), alloc_);

    vmaDestroyBuffer(device_->allocator(), buffer_, alloc_);
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
