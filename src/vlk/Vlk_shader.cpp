//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "std_lib.h"
#include "vlk_lib.h"
#include "Vlk_shader.h"
#include "Vlk_device.h"

using namespace std;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Vlk_shader::Vlk_shader(const Shader_desc& desc, Vlk_device* device) :
    Shader(),
    device_ { device },
    stage_ { desc.stage },
    shader_module_ { VK_NULL_HANDLE }
{
    init_shader_module_(desc);
}

//----------------------------------------------------------------------------------------------------------------------

Vlk_shader::~Vlk_shader()
{
    fini_shader_module();
}

//----------------------------------------------------------------------------------------------------------------------

Device* Vlk_shader::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

Stage Vlk_shader::stage() const noexcept
{
    return stage_;
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_shader::init_shader_module_(const Shader_desc& desc)
{
    // configure a shader module create info.
    VkShaderModuleCreateInfo create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = sizeof(uint32_t) * desc.src.size();
    create_info.pCode = &desc.src[0];

    // try to create a shader module.
    if (vkCreateShaderModule(device_->device(), &create_info, nullptr, &shader_module_))
        throw runtime_error("fail to create a shader");
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_shader::fini_shader_module()
{
    vkDestroyShaderModule(device_->device(), shader_module_, nullptr);
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
