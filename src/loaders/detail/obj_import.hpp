/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/geometries/geometry.hpp"
#include "vglx/math/color.hpp"

#include "shared.hpp"

#include <expected>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace vglx::detail::obj {

namespace fs = std::filesystem;

struct PhongMaterialDescriptor {
    std::string name;

    Color diffuse {1.0f, 1.0f, 1.0f};
    Color specular {0.0f, 0.0f, 0.0f};
    Color emission {0.0f, 0.0f, 0.0f};

    float shininess {32.0f};

    TextureRef tex_diffuse;
    TextureRef tex_alpha;
    TextureRef tex_normal;
    TextureRef tex_specular;
    TextureRef tex_emissive;
};

struct OBJMeshEntry {
    std::string name;
    std::shared_ptr<Geometry> geometry;
    int material_index {-1};
};

struct OBJResult {
    std::vector<OBJMeshEntry> entries;
    std::vector<PhongMaterialDescriptor> materials;
};

[[nodiscard]] auto import(const fs::path& path) -> std::expected<OBJResult, std::string>;

}
