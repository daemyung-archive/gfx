//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_STD_LIB_MODULES_GUARD
#define GFX_STD_LIB_MODULES_GUARD

#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <array>
#include <vector>
#include <unordered_map>
#include <bitset>
#include <mutex>

//----------------------------------------------------------------------------------------------------------------------

template <typename T>
inline uint32_t etoi(T e) noexcept
{
    return static_cast<uint32_t>(e);
}

//----------------------------------------------------------------------------------------------------------------------

#endif // GFX_STD_LIB_MODULES_GUARD
