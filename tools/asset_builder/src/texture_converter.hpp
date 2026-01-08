/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/asset_format.hpp"

#include <expected>
#include <filesystem>

namespace fs = std::filesystem;

auto convert_texture(
    const fs::path& input_path,
    const fs::path& output_path,
    TextureColorSpace color_space
) -> std::expected<void, std::string>;