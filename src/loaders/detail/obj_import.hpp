/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/geometries/geometry.hpp"
#include "vglx/math/color.hpp"

#include <expected>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace vglx::detail::obj {

namespace fs = std::filesystem;

struct MaterialDescriptor {
    std::string name;

    Color diffuse {1.0f, 1.0f, 1.0f};
    Color specular {0.0f, 0.0f, 0.0f};
    Color emission {0.0f, 0.0f, 0.0f};

    float shininess {32.0f};

    std::string tex_diffuse;
    std::string tex_alpha;
    std::string tex_normal;
    std::string tex_specular;
    std::string tex_emissive;
};

struct ObjEntry {
    std::string name;
    std::shared_ptr<Geometry> geometry;
    int material_index {-1};
};

struct ObjResult {
    std::vector<ObjEntry> entries;
    std::vector<MaterialDescriptor> materials;
};

[[nodiscard]] auto import(const fs::path& path) -> std::expected<ObjResult, std::string>;

}
