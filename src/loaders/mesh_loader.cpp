/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/asset_format.hpp"
#include "vglx/loaders/mesh_loader.hpp"
#include "vglx/loaders/texture_loader.hpp"
#include "vglx/geometries/geometry.hpp"
#include "vglx/materials/phong_material.hpp"
#include "vglx/math/color.hpp"
#include "vglx/scene/mesh.hpp"
#include "vglx/scene/node.hpp"
#include "vglx/textures/texture_2d.hpp"

#include "utilities/logger.hpp"
#include "utilities/file.hpp"

#include <cstring>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace vglx {

namespace {

auto configure_geometry_attributes(const MeshRecord& h, const std::shared_ptr<Geometry>& geometry) {
    geometry->SetAttribute({.type = Geometry::VertexAttributeType::Position, .item_size = 3});
    geometry->SetAttribute({.type = Geometry::VertexAttributeType::Normal, .item_size = 3});

    if (h.vertex_flags & VertexAttributeFlags::VertexAttr_HasUV) {
        geometry->SetAttribute({.type = Geometry::VertexAttributeType::UV, .item_size = 2});
    }
    if (h.vertex_flags & VertexAttributeFlags::VertexAttr_HasTangent) {
        geometry->SetAttribute({.type = Geometry::VertexAttributeType::Tangent, .item_size = 4});
    }
    if (h.vertex_flags & VertexAttributeFlags::VertexAttr_HasColor) {
        geometry->SetAttribute({.type = Geometry::VertexAttributeType::Color, .item_size = 3});
    }
}

auto load_materials(const fs::path& path, std::ifstream& file, const MeshHeader& mesh_header) {
    const auto texture_loader = TextureLoader::Create();
    auto textures = std::unordered_map<std::string, std::shared_ptr<Texture2D>> {};
    auto materials = std::vector<std::shared_ptr<Material>> {};
    materials.reserve(mesh_header.material_count);

    for (uint32_t i = 0; i < mesh_header.material_count; ++i) {
        auto material_record = MaterialRecord {};
        read_binary(file, material_record);

        auto material = PhongMaterial::Create();
        material->color = Color { material_record.diffuse };
        material->specular = Color { material_record.specular };
        material->shininess = material_record.shininess;

        for (uint32_t t = 0; t < material_record.texture_count; ++t) {
            auto texture_record = MaterialTextureMapRecord {};
            read_binary(file, texture_record);

            const auto filename = std::string {texture_record.filename};
            if (filename.empty()) continue;

            auto texture = std::shared_ptr<Texture2D> {};
            if (auto it = textures.find(filename); it != textures.end()) {
                texture = it->second;
            } else {
                const auto texture_path = path.parent_path() / filename;
                const auto result = texture_loader->Load(texture_path.string());
                if (result) {
                    texture = result.value();
                    textures.emplace(filename, texture);
                } else {
                    Logger::Log(LogLevel::Error, "{}", result.error());
                }
            }

            if (texture == nullptr) continue;

            switch (texture_record.type) {
                case MaterialTextureMapType_Diffuse:
                    material->color = 0xFFFFFF;
                    material->albedo_map = texture;
                break;
                case MaterialTextureMapType_Alpha:
                    material->alpha_map = texture;
                break;
                case MaterialTextureMapType_Normal:
                    material->normal_map = texture;
                break;
                case MaterialTextureMapType_Specular:
                    material->specular_map = texture;
                break;
                default:
                    Logger::Log(
                        LogLevel::Error,
                        "Unsupported texture type {}",
                        texture_record.type
                    );
            }
        }

        materials.emplace_back(material);
    }
    return materials;
}

auto load_mesh(const fs::path& path, std::ifstream& file, const MeshHeader& mesh_header) -> LoaderResult<std::unique_ptr<Node>> {
    auto materials = load_materials(path, file, mesh_header);
    auto root = Node::Create();

    for (uint32_t i = 0; i < mesh_header.mesh_count; ++i) {
        auto mesh_record = MeshRecord {};
        read_binary(file, mesh_record);

        if (mesh_record.vertex_count == 0 || mesh_record.index_count == 0) {
            return std::unexpected("Mesh record has zero vertices or indices");
        }

        auto vertex_data = std::vector<float>(mesh_record.vertex_count * mesh_record.vertex_stride);
        read_binary(file, vertex_data, mesh_record.vertex_data_size);

        auto index_data = std::vector<unsigned int>(mesh_record.index_count);
        read_binary(file, index_data, mesh_record.index_data_size);

        auto geometry = Geometry::Create(vertex_data, index_data);
        geometry->SetName(mesh_record.name);

        configure_geometry_attributes(mesh_record, geometry);

        auto material_idx = mesh_record.material_index;
        if (material_idx < materials.size()) {
            root->Add(Mesh::Create(geometry, materials[material_idx]));
        } else {
            root->Add(Mesh::Create(geometry, PhongMaterial::Create()));
        }
    }

    return root;
}

} // unnamed namespace

auto MeshLoader::LoadImpl(const fs::path& path) const -> LoaderResult<std::unique_ptr<Node>> {
    auto file = std::ifstream {path, std::ios::binary};
    auto path_s = path.string();
    if (!file) {
        return std::unexpected("Unable to open file '" + path_s + "'");
    }

    auto mesh_header = MeshHeader {};
    read_binary(file, mesh_header);
    if (
        std::memcmp(mesh_header.magic, "MSH0", 4) != 0 &&
        std::memcmp(mesh_header.magic, "MES0", 4) != 0
    ) {
        return std::unexpected("Invalid mesh file '" + path_s + "'");
    }

    if (mesh_header.version != VGLX_MSH_VER) {
        return std::unexpected("Unsupported mesh version in file '" + path_s + "'");
    }

    return load_mesh(path, file, mesh_header);
}

}