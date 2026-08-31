/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <cstddef>
#include <functional>

namespace vglx {

template <typename T>
inline auto HashCombine(size_t& seed, const T& value) -> void {
    seed ^= std::hash<T>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

}
