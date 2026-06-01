/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#define CGLTF_IMPLEMENTATION

#include "loaders/detail/gltf_import.hpp"

#include "misc/cgltf.hpp"

namespace vglx::detail::gltf {

auto import(const fs::path& path) -> std::expected<GLTFResult, std::string> {
    return std::unexpected("implement");
}

}
