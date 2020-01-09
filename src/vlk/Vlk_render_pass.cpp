//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "std_lib.h"
#include "vlk_lib.h"
#include "Vlk_render_pass.h"
#include "Vlk_device.h"

using namespace std;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Vlk_render_pass::Vlk_render_pass(const Vlk_render_pass_desc& desc, Vlk_device* device) :
    device_ { device },
    render_pass_ { VK_NULL_HANDLE }
{
    init_render_pass_(desc);
}

//----------------------------------------------------------------------------------------------------------------------

Vlk_render_pass::~Vlk_render_pass()
{
    fini_render_pass_();
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_render_pass::init_render_pass_(const Vlk_render_pass_desc& desc)
{
    // convert to attachment descriptions.
    vector<VkAttachmentDescription> attachments;

    {
        for (auto& color : desc.colors) {
            if (Format::invalid == color.format)
                continue;

            VkAttachmentDescription attachment {};

            attachment.format = to_VkFormat(color.format);
            attachment.samples = to_VkSampleCountFlagBits(color.samples);
            attachment.loadOp = to_VkAttachmentLoadOp(color.load_op);
            attachment.storeOp = to_VkAttachmentStoreOp(color.store_op);
            attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            attachments.push_back(attachment);
        }

        auto& depth_stencil = desc.depth_stencil;

        if (Format::invalid != depth_stencil.format) {
            VkAttachmentDescription attachment {};

            attachment.format = to_VkFormat(depth_stencil.format);
            attachment.samples = to_VkSampleCountFlagBits(depth_stencil.samples);
            attachment.loadOp = to_VkAttachmentLoadOp(depth_stencil.load_op);
            attachment.storeOp = to_VkAttachmentStoreOp(depth_stencil.store_op);
            attachment.stencilLoadOp = attachment.loadOp;
            attachment.stencilStoreOp = attachment.storeOp;
            attachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            attachments.push_back(attachment);
        }
    }

    // convert to attachment references.
    array<VkAttachmentReference, 4> color_references;
    VkAttachmentReference depth_stencil_reference {};

    {
        auto attachment { 0 };

        for (auto i = 0 ; i != 4; ++i) {
            auto& color = desc.colors[i];

            if (Format::invalid == color.format)
                color_references[i].attachment = VK_ATTACHMENT_UNUSED;
            else
                color_references[i].attachment = attachment++;

            color_references[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }

        auto& depth_stencil = desc.depth_stencil;

        if (Format::invalid == depth_stencil.format)
            depth_stencil_reference.attachment = VK_ATTACHMENT_UNUSED;
        else
            depth_stencil_reference.attachment = attachment;

        depth_stencil_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    // configure a subpass description.
    VkSubpassDescription subpass {};

    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    // subpass.colorAttachmentCount = color_references.size();
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_references[0];
    subpass.pDepthStencilAttachment = &depth_stencil_reference;

    // configure a render pass create info.
    VkRenderPassCreateInfo create_info {};

    create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    create_info.attachmentCount = attachments.size();
    create_info.pAttachments = &attachments[0];
    create_info.subpassCount = 1;
    create_info.pSubpasses = &subpass;

    // try to create a render pass.
    if (vkCreateRenderPass(device_->device(), &create_info, nullptr, &render_pass_))
        throw runtime_error("fail to create a render pass");
}

//----------------------------------------------------------------------------------------------------------------------

void Vlk_render_pass::fini_render_pass_()
{
    vkDestroyRenderPass(device_->device(), render_pass_, nullptr);
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
