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
    Swap_chain(),
    device_ { device },
    image_format_ { desc.image_format },
    image_extent_ { desc.image_extent },
    color_space_ { desc.color_space },
    frame_count_ { 0 },
    window_ { desc.window },
    layer_ { nil },
    images_ { desc.image_count },
    image_index_ { 0 },
    drawable_ { nil }
{
    init_layer_(desc);
    connect_to_window_();
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

Format Mtl_swap_chain::image_format() const
{
    return image_format_;
}

//----------------------------------------------------------------------------------------------------------------------

Extent Mtl_swap_chain::image_extent() const
{
    return image_extent_;
}

//----------------------------------------------------------------------------------------------------------------------

Color_space Mtl_swap_chain::color_space() const
{
    return color_space_;
}

//----------------------------------------------------------------------------------------------------------------------

uint64_t Mtl_swap_chain::frame_count() const
{
    return frame_count_;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_swap_chain::init_layer_(const Swap_chain_desc& desc)
{
    layer_ = [CAMetalLayer layer];

    if (!layer_)
        throw runtime_error("fail to create a swap chain");

    layer_.device = device_->device();
    layer_.pixelFormat = convert<MTLPixelFormat>(desc.image_format);
    layer_.framebufferOnly = NO;
    layer_.maximumDrawableCount = desc.image_count;
    layer_.drawableSize = CGSizeMake(desc.image_extent.w, desc.image_extent.h);
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

void Mtl_swap_chain::connect_to_window_()
{
#if TARGET_OS_IOS
    auto window = (__bridge UIWindow*)window_;

    [window.rootViewController.view.layer addSublayer:layer_];
    [layer_ setFrame:CGRectMake(0, 0, image_extent_.w, image_extent_.h)];
#elif TARGET_OS_OSX
    auto window = (__bridge NSWindow*)window_;

    [[window contentView] setLayer:layer_];
#endif
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
