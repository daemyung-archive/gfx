//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_SHADER_GUARD
#define GFX_SHADER_GUARD

#include <vector>
#include <sc/enums.h>
#include <sc/Spirv_reflector.h>
#include "enums.h"
#include "types.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Device;

//----------------------------------------------------------------------------------------------------------------------

struct Shader_desc final {
    Sc_lib::Shader_type type;
    std::vector<uint32_t> src;
};

//----------------------------------------------------------------------------------------------------------------------

class Shader {
public:
    explicit Shader(const Shader_desc& desc) :
        type_ {desc.type}
    {}

    virtual ~Shader() = default;

    virtual Device* device() const = 0;

    virtual Sc_lib::Signature reflect() const noexcept = 0;

    inline Sc_lib::Shader_type type() const noexcept
    { return type_; }

protected:
    Sc_lib::Shader_type type_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_SHADER_GUARD
