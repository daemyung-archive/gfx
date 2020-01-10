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

    void* map() override;

    void unmap() override;

    Device* device() const override;

    inline auto& buffer() const noexcept
    { return buffer_; }

private:
    void init_buffer_and_alloc_(const void* data);

    void fini_buffer_and_alloc_();

private:
    Vlk_device* device_;
    VkBuffer buffer_;
    VmaAllocation alloc_;
    void* contents_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_VLK_BUFFER_GUARD

