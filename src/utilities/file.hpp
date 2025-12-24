/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <concepts>

namespace vglx {

template <typename T>
requires std::is_trivially_copyable_v<T>
auto read_binary(std::istream& in, T& value) -> bool {
    in.read(reinterpret_cast<char*>(&value), sizeof(T));
    return in.gcount() == static_cast<std::streamsize>(sizeof(T));
}

template <typename T>
requires std::is_trivially_copyable_v<T>
auto read_binary(std::istream& in, std::vector<T>& vec, std::size_t count) -> bool {
    in.read(reinterpret_cast<char*>(vec.data()), count);
    return in.gcount() == static_cast<std::streamsize>(count);
}

}