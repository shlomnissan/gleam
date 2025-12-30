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

class Node;

namespace fs = std::filesystem;

auto load_mesh(const fs::path& path) -> std::expected<std::unique_ptr<Node>, std::string>;

}