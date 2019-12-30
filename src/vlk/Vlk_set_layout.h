//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_VLK_SET_LAYOUT_GUARD
#define GFX_VLK_SET_LAYOUT_GUARD

#include <vector>
#include <vulkan/vulkan.h>
#include "enums.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Vlk_device;

//----------------------------------------------------------------------------------------------------------------------

struct Vlk_set_layout_desc {
    std::vector<VkDescriptorSetLayoutBinding> bindings;
};

//----------------------------------------------------------------------------------------------------------------------

class Vlk_set_layout final {
public:
    Vlk_set_layout(const Vlk_set_layout_desc& desc, Vlk_device* device);

    ~Vlk_set_layout();

    inline auto& desc_set_layout() const noexcept
    { return desc_set_layout_; }

    VkDescriptorSet desc_set();

private:
    void init_desc_set_layout_(const Vlk_set_layout_desc& desc);

    void init_desc_pool_(const Vlk_set_layout_desc& desc);

    void fini_desc_set_layout_();

    void fini_desc_pool_();

private:
    Vlk_device* device_;
    VkDescriptorSetLayout desc_set_layout_;
    VkDescriptorPool desc_pool_;
    std::vector<VkDescriptorSet> desc_sets_;
    uint64_t desc_set_index_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_VLK_SET_LAYOUT_GUARD
