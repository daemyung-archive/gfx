//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "std_lib.h"
#include "vlk_lib.h"
#include "Vlk_set_layout.h"
#include "Vlk_device.h"

using namespace std;
using namespace Gfx_lib;

namespace {

//----------------------------------------------------------------------------------------------------------------------

inline vector<VkDescriptorPoolSize> to_pool_sizes(const Vlk_set_layout_desc& desc)
{
    unordered_map<VkDescriptorType, uint32_t> counts;

    for (auto& binding : desc.bindings)
        ++counts[binding.descriptorType];

    vector<VkDescriptorPoolSize> pool_sizes;

    for (auto& [type, count] : counts)
        pool_sizes.push_back({type, count});

    return pool_sizes;
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Vlk_set_layout::Vlk_set_layout(const Vlk_set_layout_desc& desc, Vlk_device* device) :
    device_ {device},
    desc_set_layout_ {VK_NULL_HANDLE},
    desc_pool_ {VK_NULL_HANDLE},
    desc_sets_ {1024},
    desc_set_index_ {0}
{
    init_desc_set_layout_(desc);
    init_desc_pool_(desc);
}

//----------------------------------------------------------------------------------------------------------------------

Vlk_set_layout::~Vlk_set_layout()
{
    fini_desc_set_layout_();
    fini_desc_pool_();
}

//----------------------------------------------------------------------------------------------------------------------

VkDescriptorSet Vlk_set_layout::desc_set()
{
    auto& desc_set = desc_sets_[desc_set_index_];

    if (!desc_set) {
        VkDescriptorSetAllocateInfo alloc_info {};

        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = desc_pool_;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &desc_set_layout_;

        vkAllocateDescriptorSets(device_->device(), &alloc_info, &desc_set);
    }

    desc_set_index_ = ++desc_set_index_ % 1024;

    return desc_set;
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_set_layout::init_desc_set_layout_(const Vlk_set_layout_desc& desc)
{
    VkDescriptorSetLayoutCreateInfo create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    create_info.bindingCount = static_cast<uint32_t>(desc.bindings.size());
    create_info.pBindings = &desc.bindings[0];

    if (vkCreateDescriptorSetLayout(device_->device(), &create_info, nullptr, &desc_set_layout_))
        throw runtime_error("");
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_set_layout::init_desc_pool_(const Vlk_set_layout_desc& desc)
{
    auto pool_sizes = to_pool_sizes(desc);

    VkDescriptorPoolCreateInfo create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    create_info.maxSets = 1024;
    create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    create_info.pPoolSizes = &pool_sizes[0];

    if (vkCreateDescriptorPool(device_->device(), &create_info, nullptr, &desc_pool_))
        throw runtime_error("");
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_set_layout::fini_desc_set_layout_()
{
    vkDestroyDescriptorSetLayout(device_->device(), desc_set_layout_, nullptr);
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_set_layout::fini_desc_pool_()
{
    vkDestroyDescriptorPool(device_->device(), desc_pool_, nullptr);
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
