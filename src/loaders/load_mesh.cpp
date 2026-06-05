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

#include "loaders/detail/gltf_import.hpp"
#include "loaders/detail/image_import.hpp"
#include "loaders/detail/obj_import.hpp"
#include "loaders/detail/shared.hpp"

#include "utilities/logger.hpp"

namespace vglx {

namespace {

auto load_texture(
    const detail::TextureRef& ref,
    const fs::path& base_dir,
    Texture::ColorSpace color_space
) -> std::shared_ptr<Texture2D> {
    if (ref.empty()) return nullptr;

    auto path = base_dir / ref.uri;
    if (fs::is_directory(path)) return nullptr;

    auto result = LoadTexture(path, color_space);
    if (!result.has_value()) {
        Logger::Log(LogLevel::Error,
            "Failed to load texture for mesh {}", path.string()
        );
        return nullptr;
    }

    auto texture = result.value();
    texture->wrap_s = ref.wrap_s;
    texture->wrap_t = ref.wrap_t;
    texture->min_filter = ref.min_filter;
    texture->mag_filter = ref.mag_filter;
    texture->transform.SetScale(ref.uv_scale);
    texture->transform.SetPosition(ref.uv_offset);
    texture->transform.SetRotation(ref.uv_rotation);

    return texture;
}

auto load_obj_material(const detail::obj::PhongMaterialDescriptor& desc, const fs::path& base_dir) {
    auto material = PhongMaterial::Create();

    material->color = desc.diffuse;
    material->specular_color = desc.specular;
    material->emissive_color = desc.emission;
    material->shininess = desc.shininess;
    material->albedo_map = load_texture(desc.tex_diffuse, base_dir, Texture::ColorSpace::sRGB);
    material->alpha_map = load_texture(desc.tex_alpha, base_dir, Texture::ColorSpace::Linear);
    material->normal_map = load_texture(desc.tex_normal, base_dir, Texture::ColorSpace::Linear);
    material->specular_map = load_texture(desc.tex_specular, base_dir, Texture::ColorSpace::Linear);
    material->emissive_map = load_texture(desc.tex_emissive, base_dir, Texture::ColorSpace::sRGB);

    if (material->albedo_map) {
        material->color = 0xFFFFFF;
    }

    return material;
}

auto load_obj_mesh(const fs::path& path) -> std::expected<std::unique_ptr<Node>, std::string> {
    auto result = detail::obj::import(path);
    if (!result) {
        return std::unexpected(result.error());
    }

    auto base_dir = path.parent_path();
    auto obj = result.value();

    auto materials = std::vector<std::shared_ptr<PhongMaterial>> {};
    materials.reserve(obj.materials.size());
    for (const auto& desc : obj.materials) {
        materials.emplace_back(load_obj_material(desc, base_dir));
    }

    auto root = std::make_unique<Node>();
    for (const auto& entry : obj.entries) {
        auto has_material = entry.material_index >= 0
          && entry.material_index < static_cast<int>(materials.size());

        auto material = has_material
            ? materials[entry.material_index]
            : PhongMaterial::Create();

        auto mesh = Mesh::Create(entry.geometry, material);
        mesh->SetName(entry.name);

        root->Add(std::move(mesh));
    }

    return root;
}

auto load_gltf_mesh(const fs::path& path) -> std::expected<std::unique_ptr<Node>, std::string> {
    auto result = detail::gltf::import(path);
    if (!result) {
        return std::unexpected(result.error());
    }

    auto root = std::make_unique<Node>();
    return root;
}

}

auto LoadMesh(
    const fs::path& path
) -> std::expected<std::unique_ptr<Node>, std::string> {
    if (!fs::exists(path)) {
        return std::unexpected(std::format("Can't find mesh {}", path.string()));
    }

    auto ext = path.extension().string();

    if (ext == ".obj") return load_obj_mesh(path);

    if (ext == ".gltf") return load_gltf_mesh(path);

    return std::unexpected(std::format("Unsupported file extension {}", ext));
}

}
