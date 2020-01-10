//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_STD_LIB_MODULES_GUARD
#define GFX_STD_LIB_MODULES_GUARD

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <array>
#include <vector>
#include <deque>
#include <unordered_map>
#include <algorithm>
#include <mutex>

//----------------------------------------------------------------------------------------------------------------------

template <typename T>
inline uint32_t etoi(T e) noexcept
{
    return static_cast<uint32_t>(e);
}

//----------------------------------------------------------------------------------------------------------------------

template<typename R, typename F>
inline F for_each(R& range, F func)
{
    return std::for_each(std::begin(range), std::end(range), func);
}

//----------------------------------------------------------------------------------------------------------------------

template<typename R, typename O, typename F>
inline O transform(R& range, O iter, F func)
{
    return std::transform(std::begin(range), std::end(range), iter, func);
}

//----------------------------------------------------------------------------------------------------------------------

#endif // GFX_STD_LIB_MODULES_GUARD
