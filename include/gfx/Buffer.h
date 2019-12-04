//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_BUFFER_GUARD
#define GFX_BUFFER_GUARD

#include <cstddef>
#include <gsl/gsl>
#include "gfx/enums.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

struct Buffer_desc {
    const void* data { nullptr };
    size_t size { 0 };
    Heap_type type { Heap_type::upload };
};

//----------------------------------------------------------------------------------------------------------------------

class Buffer {
public:
    virtual ~Buffer() = default;

    virtual gsl::span<std::byte> map() = 0;

    virtual void unmap() = 0;

    virtual Heap_type type() const = 0;

    virtual uint64_t size() const = 0;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_BUFFER_GUARD
