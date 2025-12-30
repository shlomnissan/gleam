/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/loaders/mesh_loader_xyz.hpp"

#include "vglx/asset_format.hpp"
#include "vglx/core/load_scheduler.hpp"
#include "vglx/geometries/geometry.hpp"
#include "vglx/loaders/texture_loader_xyz.hpp"
#include "vglx/materials/material.hpp"
#include "vglx/materials/phong_material.hpp"
#include "vglx/math/color.hpp"
#include "vglx/scene/mesh.hpp"
#include "vglx/scene/node.hpp"
#include "vglx/textures/texture_2d.hpp"

#include "utilities/assert.hpp"
#include "utilities/file.hpp"
#include "utilities/logger.hpp"

#include <cstdint>
#include <cstring>
#include <format>
#include <fstream>
#include <unordered_map>
#include <vector>

namespace vglx {

namespace {

auto process_materials(
    const MeshHeader& header,
    const fs::path path,
    std::ifstream& file
) ->  std::expected<std::vector<std::shared_ptr<Material>>, std::string> {
    auto textures = std::unordered_map<std::string, std::shared_ptr<Texture2D>> {};
    auto materials = std::vector<std::shared_ptr<Material>> {};
    materials.reserve(header.material_count);

    for (uint32_t i = 0; i < header.material_count; ++i) {
        auto mat_record = MaterialRecord {};
        if (!read_binary(file, mat_record)) {
            return std::unexpected(std::format("Failed to read material from '{}'", path.string()));
        }

        auto material = PhongMaterial::Create();
        material->color = Color { mat_record.diffuse };
        material->specular = Color { mat_record.specular };
        material->shininess = mat_record.shininess;

        for (uint32_t t = 0; t < mat_record.texture_count; ++t) {
            auto tex_record = MaterialTextureMapRecord {};
            if (!read_binary(file, tex_record)) {
                return std::unexpected(std::format("Failed to read texture from '{}'", path.string()));
            }

            const auto filename = std::string {tex_record.filename};
            const auto texture_path = path.parent_path() / filename;

            if (auto it = textures.find(filename); it != textures.end()) {
                textures.emplace(filename, it->second);
            } else {
                const auto texture_path = path.parent_path() / filename;
                const auto result = load_texture(texture_path.string());
                if (!result) {
                    return std::unexpected(result.error());
                }
                textures.emplace(filename, result.value());
            }

            switch(tex_record.type) {
            case MaterialTextureMapType_Diffuse:
                material->color = Color {0xFFFFFF};
                material->albedo_map = textures.at(filename);
            break;
            case MaterialTextureMapType_Alpha:
                material->alpha_map = textures.at(filename);
            break;
            case MaterialTextureMapType_Normal:
                material->normal_map = textures.at(filename);
            break;
            case MaterialTextureMapType_Specular:
                material->specular_map = textures.at(filename);
            break;
            default:
                return std::unexpected(std::format("Unsupported texture type {}", tex_record.type));
            }
        }

        materials.emplace_back(material);
    }

    return materials;
}

auto process_mesh(
    const MeshHeader& header,
    const fs::path path,
    std::ifstream& file
) -> std::expected<std::unique_ptr<Node>, std::string> {
    auto result = process_materials(header, path, file);
    if (!result) {
        std::unexpected(result.error());
    }

    auto materials = result.value();
    auto root = Node::Create();

    for (uint32_t i = 0; i < header.mesh_count; ++i) {
        auto msh_record = MeshRecord {};
        if (!read_binary(file, msh_record)) {
            return std::unexpected(std::format("Failed to read mesh record from '{}'", path.string()));
        }

        if (msh_record.vertex_count == 0 || msh_record.index_count == 0) {
            return std::unexpected(std::format("Mesh record has zero vertices or indices '{}'", path.string()));
        }

        auto vertex_data = std::vector<float>(msh_record.vertex_count * msh_record.vertex_stride);
        if (!read_binary(file, vertex_data, msh_record.vertex_data_size)) {
            return std::unexpected(std::format("Failed to read vertex data '{}'", path.string()));
        }

        auto index_data = std::vector<unsigned int>(msh_record.index_count);
        if (!read_binary(file, index_data, msh_record.index_data_size)) {
            return std::unexpected(std::format("Failed to read index data '{}'", path.string()));
        }

        auto geometry = Geometry::Create(vertex_data, index_data);
        geometry->SetName(msh_record.name);

        geometry->SetAttribute({.type = Geometry::VertexAttributeType::Position, .item_size = 3});
        geometry->SetAttribute({.type = Geometry::VertexAttributeType::Normal, .item_size = 3});
        if (msh_record.vertex_flags & VertexAttributeFlags::VertexAttr_HasUV) {
            geometry->SetAttribute({.type = Geometry::VertexAttributeType::UV, .item_size = 2});
        }
        if (msh_record.vertex_flags & VertexAttributeFlags::VertexAttr_HasTangent) {
            geometry->SetAttribute({.type = Geometry::VertexAttributeType::Tangent, .item_size = 4});
        }
        if (msh_record.vertex_flags & VertexAttributeFlags::VertexAttr_HasColor) {
            geometry->SetAttribute({.type = Geometry::VertexAttributeType::Color, .item_size = 3});
        }

        auto material_idx = msh_record.material_index;
        auto material = material_idx < materials.size()
            ? materials[material_idx]
            : PhongMaterial::Create();

        root->Add(Mesh::Create(geometry, material));
    }

    return root;
}

auto import(const fs::path& path) -> std::expected<std::unique_ptr<Node>, std::string> {
    auto file = std::ifstream {path, std::ios::binary};
    if (!file) {
        return std::unexpected(std::format("Unable to open file '{}'", path.string()));
    }

    auto header = MeshHeader {};
    if (!read_binary(file, header)) {
        return std::unexpected(std::format("Failed to read header from '{}'", path.string()));
    }

    if (std::memcmp(header.magic, "MSH0", 4) != 0) {
        return std::unexpected(std::format("Invalid mesh file '{}'", path.string()));
    }

    if (header.version != VGLX_MSH_VER) {
        return std::unexpected(std::format("Unsupported file version '{}'", path.string()));
    }

    return process_mesh(header, path, file);
}

} // anonymous namespace

MeshLoaderXYZ::MeshLoaderXYZ(LoadScheduler* scheduler) : load_scheduler_(scheduler) {};

auto MeshLoaderXYZ::Load(const fs::path& path)
  -> std::expected<std::shared_ptr<Node>, std::string> {
    return import(path);
}

auto MeshLoaderXYZ::LoadAsync(const fs::path& path) -> MeshLoadHandle {
    VGLX_ASSERT(load_scheduler_ != nullptr, "Null load scheduler in mesh loader");

    auto state = std::make_shared<MeshLoadHandle::State>();
    auto handle = MeshLoadHandle {state};

    load_scheduler_->Enqueue(
        [state, path] {
            auto result = import(path);
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