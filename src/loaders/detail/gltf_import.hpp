/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <filesystem>

namespace vglx::detail::gltf {

namespace fs = std::filesystem;

auto import(const fs::path& path) -> void;

}
