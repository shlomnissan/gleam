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

#include "vglx/textures/image.hpp"

namespace vglx::detail::image {

namespace fs = std::filesystem;

[[nodiscard]] auto import(const fs::path& path) -> std::expected<std::shared_ptr<Image>, std::string>;

}