//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_MTL_BUFFER_GUARD
#define GFX_MTL_BUFFER_GUARD

#include <Metal/Metal.h>
#include "Buffer.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Mtl_device;

//----------------------------------------------------------------------------------------------------------------------

class Mtl_buffer final : public Buffer {
public:
    Mtl_buffer(const Buffer_desc& desc, Mtl_device* device);

    gsl::span<std::byte> map() override;

    void unmap() override;

    Device* device() const override;

    Heap_type type() const override;

    uint64_t size() const override;

    inline id<MTLBuffer> buffer() const noexcept
    { return buffer_; }

private:
    void init_buffer_(const Buffer_desc& desc);

private:
    Mtl_device* device_;
    Heap_type type_;
    id<MTLBuffer> buffer_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_MTL_BUFFER_GUARD

