/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/geometries/geometry.hpp"
#include "vglx/math/color.hpp"
#include "vglx/math/matrix4.hpp"

#include "texture_ref.hpp"

#include <expected>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace vglx::detail::gltf {

namespace fs = std::filesystem;

struct PBRMaterialDescriptor {
    std::string name;

    Color base_color {1.0f, 1.0f, 1.0f};
    Color emissive {0.0f, 0.0f, 0.0f};

    float metallic {1.0f};
    float roughness {1.0f};
    float emissive_intensity {1.0f};

    float ao_intensity {1.0f};
    float normal_intensity {1.0f};

    TextureRef tex_base_color;
    TextureRef tex_metallic_roughness;
    TextureRef tex_normal;
    TextureRef tex_occlusion;
    TextureRef tex_emissive;
};

struct GLTFPrimitive {
    std::shared_ptr<Geometry> geometry;
    int material_index {-1};
};

struct GLTFNodeEntry {
    std::string name;
    Matrix4 transform {Matrix4::Identity()};
    std::vector<int> children;
    std::vector<GLTFPrimitive> primitives;
};

struct GLTFResult {
    std::vector<GLTFNodeEntry> nodes;
    std::vector<int> roots;
    std::vector<PBRMaterialDescriptor> materials;
};

[[nodiscard]] auto import(const fs::path& path) -> std::expected<GLTFResult, std::string>;

}
