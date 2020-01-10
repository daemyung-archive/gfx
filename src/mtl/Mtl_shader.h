//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_MTL_SHADER_GUARD
#define GFX_MTL_SHADER_GUARD

#include <Metal/Metal.h>
#include "Shader.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Mtl_device;

//----------------------------------------------------------------------------------------------------------------------

class Mtl_shader final : public Shader {
public:
    Mtl_shader(const Shader_desc& desc, Mtl_device* device);

    Device* device() const override;

    Sc_lib::Signature reflect() const noexcept override;

    inline auto function() const noexcept
    { return function_; }

private:
    void init_signature_(const std::vector<uint32_t>& src);

    void init_function_(const std::string& src);

private:
    Mtl_device* device_;
    Sc_lib::Signature signature_;
    id<MTLFunction> function_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_MTL_SHADER_GUARD
