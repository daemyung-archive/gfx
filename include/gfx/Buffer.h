//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_BUFFER_GUARD
#define GFX_BUFFER_GUARD

#include <cstddef>
#include "enums.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Device;

//----------------------------------------------------------------------------------------------------------------------

struct Buffer_desc {
    const void* data {nullptr};
    uint64_t size {0};
    Heap_type heap_type {Heap_type::upload};
};

//----------------------------------------------------------------------------------------------------------------------

class Buffer {
public:
    explicit Buffer(const Buffer_desc& desc) noexcept :
        size_ {desc.size},
        heap_type_ {desc.heap_type}
    {}

    virtual ~Buffer() = default;

    virtual void* map() = 0;

    virtual void unmap() = 0;

    virtual Device* device() const = 0;

    inline uint64_t size() const noexcept
    { return size_; }

    inline Heap_type heap_type() const noexcept
    { return heap_type_; }

protected:
    uint64_t size_;
    Heap_type heap_type_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_BUFFER_GUARD
