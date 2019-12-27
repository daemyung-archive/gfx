//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "std_lib.h"
#include "vlk_lib.h"
#include "Vlk_framebuffer.h"
#include "Vlk_device.h"
#include "Vlk_image.h"
#include "Vlk_render_pass.h"

using namespace std;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Vlk_framebuffer::Vlk_framebuffer(const Vlk_framebuffer_desc& desc, Vlk_device* device) :
    device_ { device },
    extent_ { 0, 0, 1 },
    framebuffer_ { VK_NULL_HANDLE }
{
    init_extent_(desc);
    init_framebuffer_(desc);
}

//----------------------------------------------------------------------------------------------------------------------

Vlk_framebuffer::~Vlk_framebuffer()
{
    fini_framebuffer_();
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_framebuffer::init_extent_(const Vlk_framebuffer_desc& desc)
{
    extent_ = desc.colors[0]->extent();

    // check all attachments has a same extent.
    for (auto i = 1; i != 4; ++i) {
        auto& color = desc.colors[i];

        if (!color)
            continue;

        if (color->extent() != extent_)
            throw runtime_error("fail to create a framebuffer");
    }

    auto& depth_stencil = desc.depth_stencil;

    if (depth_stencil) {
        if (depth_stencil->extent() != extent_)
            throw runtime_error("fail to create a framebuffer");
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_framebuffer::init_framebuffer_(const Vlk_framebuffer_desc& desc)
{
    // collect image view.
    vector<VkImageView> image_views;

    for (auto& color : desc.colors) {
        if (!color)
            continue;

        image_views.push_back(color->image_view());
    }

    auto& depth_stencil = desc.depth_stencil;

    if (depth_stencil)
        image_views.push_back(depth_stencil->image_view());

    // configure a framebuffer create info.
    VkFramebufferCreateInfo create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    create_info.renderPass = desc.render_pass->render_pass();
    create_info.attachmentCount = image_views.size();
    create_info.pAttachments = &image_views[0];
    create_info.width = extent_.w;
    create_info.height = extent_.h;
    create_info.layers = 1;

    // try to create a framebuffer.
    if (vkCreateFramebuffer(device_->device(), &create_info, nullptr, &framebuffer_))
        throw runtime_error("fail to create a framebuffer");
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_framebuffer::fini_framebuffer_()
{
    vkDestroyFramebuffer(device_->device(), framebuffer_, nullptr);
}

//----------------------------------------------------------------------------------------------------------------------
    
} // of namespace Gfx_lib
