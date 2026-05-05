/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/loaders/mesh_loader.hpp"

#include "vglx/loaders/load_scheduler.hpp"
#include "vglx/materials/phong_material.hpp"
#include "vglx/scene/mesh.hpp"
#include "vglx/scene/node.hpp"
#include "vglx/textures/texture_2d.hpp"

#include "loaders/detail/image_import.hpp"
#include "loaders/detail/obj_import.hpp"

#include "utilities/assert.hpp"
#include "utilities/logger.hpp"

namespace vglx {

namespace {

auto load_texture(const std::string& path, const fs::path& base_dir, Texture::ColorSpace color_space) {
    if (path.empty()) return std::shared_ptr<Texture2D> {nullptr};

    auto full_path = base_dir / path;
    auto result = detail::image::import(full_path);
    if (!result) {
        Logger::Log(LogLevel::Error, "Failed to load texture: {}", full_path.string());
        return std::shared_ptr<Texture2D> {nullptr};
    }

    auto texture = Texture2D::Create(result.value());
    texture->color_space = color_space;
    texture->generate_mipamps = true;
    texture->min_filter = Texture::MinFilter::LinearMipmapLinear;

    return texture;
}

auto build_material(const detail::obj::MaterialDescriptor& desc, const fs::path& base_dir) {
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

auto load_mesh(const fs::path& path) -> std::expected<std::unique_ptr<Node>, std::string> {
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

MeshLoader::MeshLoader(LoadScheduler* scheduler) : load_scheduler_(scheduler) {};

auto MeshLoader::Load(const fs::path& path) const
  -> std::expected<std::unique_ptr<Node>, std::string> {
    return load_mesh(path);
}

auto MeshLoader::LoadAsync(const fs::path& path) const -> MeshLoadHandle {
    VGLX_ASSERT(load_scheduler_ != nullptr, "Null load scheduler in mesh loader");

    auto state = std::make_shared<MeshLoadHandle::State>();
    auto handle = MeshLoadHandle {state};

    load_scheduler_->Enqueue(
        [state, path] {
            auto result = load_mesh(path);
            if (result.has_value()) {
                state->value = std::move(result.value());
            } else {
                state->error = result.error();
                Logger::Log(LogLevel::Error, "{}", state->error);
            }
        },
        [state] {
            VGLX_ASSERT(state != nullptr, "Null in async mesh state");
            state->ready = true;
        }
    );

    return handle;
}

}