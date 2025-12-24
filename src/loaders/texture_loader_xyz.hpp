/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <expected>
#include <filesystem>
#include <memory>
#include <string>

namespace vglx {

class Texture2D;

namespace fs = std::filesystem;

auto load_texture(const fs::path& path) -> std::expected<std::shared_ptr<Texture2D>, std::string>;

}