//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include <platform/build_target.h>
#include "Device.h"

#if TARGET_OS_IOS || TARGET_OS_OSX
    #include "Mtl_device.h"
#elif defined(__ANDROID__)
    #include "Vlk_device.h"
#endif

using namespace std;

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Device> Device::make()
{
#if TARGET_OS_IOS || TARGET_OS_OSX
    return make_unique<Mtl_device>();
#elif defined(__ANDROID__)
    return make_unique<Vlk_device>();
#endif
    return nullptr;
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
