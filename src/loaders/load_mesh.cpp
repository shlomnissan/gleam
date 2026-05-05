/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/loaders.hpp"

#include "vglx/materials/phong_material.hpp"
#include "vglx/scene/mesh.hpp"
#include "vglx/scene/node.hpp"
#include "vglx/textures/texture_2d.hpp"

#include "loaders/detail/image_import.hpp"
#include "loaders/detail/obj_import.hpp"

#include "utilities/logger.hpp"

namespace vglx {

namespace {

auto load_texture(const fs::path& path, Texture::ColorSpace color_space) -> std::shared_ptr<Texture2D> {
    if (fs::is_directory(path)) return nullptr;

    auto texture = LoadTexture(path, color_space);
    if (texture.has_value()) {
        return texture.value();
    }

    Logger::Log(LogLevel::Error,
        "Failed to load texture for mesh {}", path.string()
    );

    return nullptr;
}

auto build_material(const detail::obj::MaterialDescriptor& desc, const fs::path& base_dir) {
    auto material = PhongMaterial::Create();

    material->color = desc.diffuse;
    material->specular_color = desc.specular;
    material->emissive_color = desc.emission;
    material->shininess = desc.shininess;
    material->albedo_map = load_texture(base_dir / desc.tex_diffuse, Texture::ColorSpace::sRGB);
    material->alpha_map = load_texture(base_dir / desc.tex_alpha, Texture::ColorSpace::Linear);
    material->normal_map = load_texture(base_dir / desc.tex_normal, Texture::ColorSpace::Linear);
    material->specular_map = load_texture(base_dir / desc.tex_specular, Texture::ColorSpace::Linear);
    material->emissive_map = load_texture(base_dir / desc.tex_emissive, Texture::ColorSpace::sRGB);

    if (material->albedo_map) {
        material->color = 0xFFFFFF;
    }

    return material;
}

}

auto LoadMesh(
    const fs::path& path
) -> std::expected<std::unique_ptr<Node>, std::string> {
    if (!fs::exists(path)) {
        return std::unexpected(std::format("Can't find mesh {}", path.string()));
    }

    auto ext = path.extension().string();
    if (ext != ".obj") {
        return std::unexpected(std::format("Unsupported file extension {}", ext));
    }

    auto result = detail::obj::import(path);
    if (!result) {
        return std::unexpected(result.error());
    }

    auto base_dir = path.parent_path();
    auto obj = result.value();

    auto materials = std::vector<std::shared_ptr<PhongMaterial>> {};
    materials.reserve(obj.materials.size());
    for (const auto& desc : obj.materials) {
        materials.emplace_back(build_material(desc, base_dir));
    }

    auto root = std::make_unique<Node>();
    for (const auto& entry : obj.entries) {
        auto has_material = entry.material_index >= 0
          && entry.material_index < static_cast<int>(materials.size());

        auto material = has_material
            ? materials[entry.material_index]
            : PhongMaterial::Create(0xFFFFFF);

        auto mesh = Mesh::Create(entry.geometry, material);
        mesh->SetName(entry.name);

        root->Add(std::move(mesh));
    }

    return root;
}

}