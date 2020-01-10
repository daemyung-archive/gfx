//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include <platform/Window.h>
#include "std_lib.h"
#include "mtl_lib.h"
#include "Mtl_swap_chain.h"
#include "Mtl_device.h"
#include "Mtl_image.h"

using namespace std;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Mtl_swap_chain::Mtl_swap_chain(const Swap_chain_desc& desc, Mtl_device* device) :
    Swap_chain {desc},
    device_ {device},
    layer_ { nil },
    images_ { desc.image_count },
    image_index_ { 0 },
    drawable_ { nil }
{
    init_layer_();
    connect_to_window_(desc.window);
    init_images();
}

//----------------------------------------------------------------------------------------------------------------------

Image* Mtl_swap_chain::acquire()
{
    auto& image = images_[image_index_];

    drawable_ = drawable_ ? drawable_ : [layer_ nextDrawable];
    image->texture_ = drawable_.texture;

    return image.get();
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_swap_chain::present()
{
    auto command_buffer = [device_->command_queue() commandBuffer];

    [command_buffer presentDrawable:drawable_];
    [command_buffer commit];

    images_[image_index_]->texture_ = nil;
    drawable_ = nil;
    image_index_ = ++frame_count_ % images_.size();
}

//----------------------------------------------------------------------------------------------------------------------

Device* Mtl_swap_chain::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_swap_chain::init_layer_()
{
    layer_ = [CAMetalLayer layer];

    if (!layer_)
        throw runtime_error("fail to create a swap chain");

    layer_.device = device_->device();
    layer_.pixelFormat = to_MTLPixelFormat(image_format_);
    layer_.framebufferOnly = NO;
    layer_.maximumDrawableCount = images_.size();
    layer_.drawableSize = CGSizeMake(image_extent_.w, image_extent_.h);
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_swap_chain::init_images()
{
    Image_desc desc;

    desc.type = Image_type::swap_chain;
    desc.format = image_format_;
    desc.extent = image_extent_;

    for (auto& image : images_) {
        try {
            image = make_unique<Mtl_image>(desc, device_);
        }
        catch (exception& except) {
            throw runtime_error("fail to create a swap chain");
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_swap_chain::connect_to_window_(void* window)
{
#if TARGET_OS_IOS
    [(__bridge UIWindow*)window_.rootViewController.view.layer addSublayer:layer_];
    [layer_ setFrame:CGRectMake(0, 0, image_extent_.w, image_extent_.h)];
#elif TARGET_OS_OSX
    [[(__bridge NSWindow*)window contentView] setLayer:layer_];
#endif
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
