//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_SHADER_GUARD
#define GFX_SHADER_GUARD

#include <vector>
#include <sc/enums.h>
#include "gfx/enums.h"
#include "gfx/types.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Device;

//----------------------------------------------------------------------------------------------------------------------

struct Shader_desc {
    Stage stage;
    std::vector<uint32_t> src;
};

//----------------------------------------------------------------------------------------------------------------------

class Shader {
public:
    virtual ~Shader() = default;

    virtual Device* device() const = 0;

    virtual Stage stage() const noexcept = 0;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_SHADER_GUARD
