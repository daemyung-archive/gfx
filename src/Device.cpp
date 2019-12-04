//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "gfx/Device.h"
#include "mtl/Mtl_device.h"

using namespace std;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Device> Device::make()
{
    return make_unique<Mtl_device>();
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
