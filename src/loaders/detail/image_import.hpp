/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <expected>
#include <filesystem>
#include <string>

#include "vglx/textures/image.hpp"

namespace vglx::detail::image {

namespace fs = std::filesystem;

[[nodiscard]] auto import(const fs::path& path) -> std::expected<Image, std::string>;

}