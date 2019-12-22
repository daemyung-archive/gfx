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
    Buffer(),
    device_ { device },
    type_ { desc.type },
    size_ { desc.size },
    buffer_ { VK_NULL_HANDLE },
    alloc_ { VK_NULL_HANDLE }
{
    init_buffer_and_alloc_(desc);
}

//----------------------------------------------------------------------------------------------------------------------

Vlk_buffer::~Vlk_buffer()
{
    fini_buffer_and_alloc_();
}

//----------------------------------------------------------------------------------------------------------------------

gsl::span<std::byte> Vlk_buffer::map()
{
    void* ptr { nullptr };

    // get the virtual memory address of a buffer.
    vmaMapMemory(device_->allocator(), alloc_, &ptr);

    assert(ptr);
    return { static_cast<byte*>(ptr), static_cast<ptrdiff_t>(size_) };
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_buffer::unmap()
{
    vmaUnmapMemory(device_->allocator(), alloc_);
    flush_alloc_();
}

//----------------------------------------------------------------------------------------------------------------------

Device* Vlk_buffer::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

Heap_type Vlk_buffer::type() const
{
    return type_;
}

//----------------------------------------------------------------------------------------------------------------------

uint64_t Vlk_buffer::size() const
{
    return size_;
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_buffer::init_buffer_and_alloc_(const Buffer_desc& desc)
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
    create_info.size = desc.size;
    create_info.usage = usage;

    // configure an allocation create info.
    VmaAllocationCreateInfo alloc_create_info {};

    alloc_create_info.usage = convert<VmaMemoryUsage>(desc.type);
    alloc_create_info.flags = (Heap_type::local == desc.type) ? 0 : VMA_ALLOCATION_CREATE_MAPPED_BIT;

    // try to create a buffer and an allocation.
    if (vmaCreateBuffer(device_->allocator(), &create_info, &alloc_create_info, &buffer_, &alloc_, nullptr))
        throw runtime_error("fail to create buffer");

    if (desc.data && desc.size) {
        memcpy(map().data(), desc.data, desc.size);
        unmap();
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_buffer::fini_buffer_and_alloc_()
{
    vmaDestroyBuffer(device_->allocator(), buffer_, alloc_);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_buffer::flush_alloc_() const
{
    VmaAllocationInfo alloc_info;

    vmaGetAllocationInfo(device_->allocator(), alloc_, &alloc_info);
    vmaFlushAllocation(device_->allocator(), alloc_, alloc_info.offset, alloc_info.size);
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
