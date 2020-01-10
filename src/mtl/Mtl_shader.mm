//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include <sc/Msl_compiler.h>
#include "mtl_lib.h"
#include "Mtl_shader.h"
#include "Mtl_device.h"

using namespace std;
using namespace Sc_lib;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Mtl_shader::Mtl_shader(const Shader_desc& desc, Mtl_device* device) :
    Shader {desc},
    device_ {device},
    signature_ {},
    function_ {nil}
{
    init_signature_(desc.src);
    init_function_(Msl_compiler().compile(desc.src));
}

//----------------------------------------------------------------------------------------------------------------------

Device* Mtl_shader::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

Sc_lib::Signature Mtl_shader::reflect() const noexcept
{
    return signature_;
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_shader::init_signature_(const std::vector<uint32_t>& src)
{
    signature_ = Spirv_reflector().reflect(src);
}

//----------------------------------------------------------------------------------------------------------------------

void Mtl_shader::init_function_(const std::string& src)
{
    // create a library.
    NSError* error;
    auto lib = [device_->device() newLibraryWithSource:@(&src[0])
                                               options:nullptr
                                                 error:&error];

    if (error)
        throw runtime_error("fail to create shader");

    // retrieve a function from a library.
    function_ = [lib newFunctionWithName:@("main0")];
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
