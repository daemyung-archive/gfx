//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_MTL_SWAP_CHAIN_GUARD
#define GFX_MTL_SWAP_CHAIN_GUARD

#include <memory>
#include <vector>
#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>
#include "Swap_chain.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Mtl_device;
class Mtl_image;

//----------------------------------------------------------------------------------------------------------------------

class Mtl_swap_chain final : public Swap_chain {
public:
    Mtl_swap_chain(const Swap_chain_desc& desc, Mtl_device* device);

    Image* acquire() override;

    void present() override;

    Device* device() const override;

private:
    void init_layer_();

    void init_images();

    void connect_to_window_(void* window);

private:
    Mtl_device* device_;
    CAMetalLayer* layer_;
    std::vector<std::unique_ptr<Mtl_image>> images_;
    uint32_t image_index_;
    id<CAMetalDrawable> drawable_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_MTL_SWAP_CHAIN_GUARD
