/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "loaders/mesh_loader_xyz.hpp"

#include "vglx/asset_format.hpp"
#include "vglx/scene/node.hpp"

#include "utilities/file.hpp"

namespace vglx {

auto load_mesh(const fs::path& path) -> std::expected<std::unique_ptr<Node>, std::string> {
    return std::unexpected(std::format("Unabled to open mesh '{}'", path.string()));
}

}