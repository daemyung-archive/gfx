//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_VLK_BUFFER_GUARD
#define GFX_VLK_BUFFER_GUARD

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include "gfx/Buffer.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Vlk_device;

//----------------------------------------------------------------------------------------------------------------------

class Vlk_buffer final : public Buffer {
public:
    Vlk_buffer(const Buffer_desc& desc, Vlk_device* device);

    ~Vlk_buffer();

    gsl::span<std::byte> map() override;

    void unmap() override;

    Device* device() const override;

    Heap_type type() const override;

    uint64_t size() const override;

    inline auto& buffer() const noexcept
    { return buffer_; }

private:
    void init_buffer_and_alloc_(const Buffer_desc& desc);

    void fini_buffer_and_alloc_();

    void flush_alloc_() const;

private:
    Vlk_device* device_;
    Heap_type type_;
    uint64_t size_;
    VkBuffer buffer_;
    VmaAllocation alloc_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_VLK_BUFFER_GUARD

