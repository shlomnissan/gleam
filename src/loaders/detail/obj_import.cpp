/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#define TINYOBJLOADER_IMPLEMENTATION

#include "loaders/detail/obj_import.hpp"

#include "vglx/math/utilities.hpp"
#include "vglx/math/vector2.hpp"
#include "vglx/math/vector3.hpp"

#include <format>
#include <unordered_map>

#include "misc/tiny_obj_loader.hpp"

namespace vglx::detail::obj {

namespace {

struct VertexKey {
    int pos_idx;
    int uv_idx;
    auto operator<=>(const VertexKey&) const = default;
};

struct VertexKeyHash {
    auto operator()(const VertexKey& key) const {
        auto h1 = std::hash<int>{}(key.pos_idx);
        auto h2 = std::hash<int>{}(key.uv_idx);
        return h1 ^ (h2 << 1);
    }
};

using VertexMap = std::unordered_map<VertexKey, unsigned, VertexKeyHash>;

auto make_texture_ref(
    const std::string& name,
    const tinyobj::texture_option_t& opt
) -> TextureRef {
    auto ref = TextureRef {};
    ref.uri = name;

    const auto wrap = opt.clamp ?
        Texture::Wrapping::ClampToEdge :
        Texture::Wrapping::Repeat;

    ref.wrap_s = wrap;
    ref.wrap_t = wrap;

    ref.uv_offset = {
        static_cast<float>(opt.origin_offset[0]),
        static_cast<float>(opt.origin_offset[1])
    };

    ref.uv_scale = {
        static_cast<float>(opt.scale[0]),
        static_cast<float>(opt.scale[1])
    };

    return ref;
}

auto parse_material(const tinyobj::material_t& material) -> PhongMaterialDescriptor {
    auto desc = PhongMaterialDescriptor {};

    desc.name = material.name;
    desc.diffuse = {material.diffuse[0], material.diffuse[1], material.diffuse[2]};
    desc.specular = {material.specular[0], material.specular[1], material.specular[2]};
    desc.emission = {material.emission[0], material.emission[1], material.emission[2]};
    desc.shininess = material.shininess;

    // If a specular map exists but Ks is black, use a small default
    // so highlights aren't completely suppressed when the map drives specular.
    if (!material.specular_texname.empty() && material.specular[0] == 0.0f) {
        desc.specular = {0.1f, 0.1f, 0.1f};
    }

    // If an emissive map exists but Ke is black, default to white
    // so the map isn't multiplied to zero.
    if (!material.emissive_texname.empty() && material.emission[0] == 0.0f) {
        desc.emission = {1.0f, 1.0f, 1.0f};
    }

    desc.tex_diffuse = make_texture_ref(material.diffuse_texname, material.diffuse_texopt);
    desc.tex_alpha = make_texture_ref(material.alpha_texname, material.alpha_texopt);
    desc.tex_specular = make_texture_ref(material.specular_texname, material.specular_texopt);
    desc.tex_emissive = make_texture_ref(material.emissive_texname, material.emissive_texopt);

    // Prefer an explicit normal map, falling back to the bump map slot.
    desc.tex_normal = material.normal_texname.empty()
        ? make_texture_ref(material.bump_texname, material.bump_texopt)
        : make_texture_ref(material.normal_texname, material.normal_texopt);

    return desc;
}

auto parse_shape(
    const tinyobj::shape_t& shape,
    const tinyobj::attrib_t& attrib
) -> OBJMeshEntry {
    auto& mesh = shape.mesh;

    auto seen_vertices = VertexMap {};
    auto vertex_data = std::vector<float> {};
    auto index_data = std::vector<unsigned> {};
    auto has_colors = !attrib.colors.empty();
    auto has_uvs = false;
    for (auto& idx : mesh.indices) {
        if (idx.texcoord_index >= 0) has_uvs = true;
    }

    auto layout = make_layout(has_uvs, has_colors);

    for (size_t i = 0; i < mesh.indices.size(); ++i) {
        const auto idx = mesh.indices[i];

        auto key = VertexKey {idx.vertex_index, idx.texcoord_index};
        if (seen_vertices.contains(key)) {
            index_data.push_back(seen_vertices[key]);
            continue;
        }

        seen_vertices[key] = static_cast<unsigned>(vertex_data.size() / layout.stride);
        index_data.push_back(seen_vertices[key]);

        vertex_data.insert(vertex_data.end(), {
            attrib.vertices[3 * idx.vertex_index + 0],
            attrib.vertices[3 * idx.vertex_index + 1],
            attrib.vertices[3 * idx.vertex_index + 2]
        });

        // placeholder for normals, always generated dynamically
        vertex_data.insert(vertex_data.end(), {0.0f, 0.0f, 0.0f});

        if (layout.has_uvs) {
            if (idx.texcoord_index >= 0) {
                vertex_data.insert(vertex_data.end(), {
                    attrib.texcoords[2 * idx.texcoord_index + 0],
                    attrib.texcoords[2 * idx.texcoord_index + 1]
                });
            } else {
                vertex_data.insert(vertex_data.end(), {0.0f, 0.0f});
            }
        }

        if (layout.has_tangents) {
            // placeholder for tangents, always generated dynamically
            vertex_data.insert(vertex_data.end(), {0.0f, 0.0f, 0.0f, 0.0f});
        }

        if (layout.has_colors) {
            vertex_data.insert(vertex_data.end(), {
                attrib.colors[3 * idx.vertex_index + 0],
                attrib.colors[3 * idx.vertex_index + 1],
                attrib.colors[3 * idx.vertex_index + 2]
            });
        }
    }

    generate_normals(vertex_data, index_data, layout);

    if (layout.has_tangents) {
        generate_tangents(vertex_data, index_data, layout);
    }

    auto name = shape.name;

    auto geometry = Geometry::Create(vertex_data, index_data);
    geometry->SetName(name);
    geometry->SetAttribute({.type = Geometry::VertexAttributeType::Position, .item_size = 3});
    geometry->SetAttribute({.type = Geometry::VertexAttributeType::Normal, .item_size = 3});

    if (layout.has_uvs) {
        geometry->SetAttribute({.type = Geometry::VertexAttributeType::UV, .item_size = 2});
    }

    if (layout.has_tangents) {
        geometry->SetAttribute({.type = Geometry::VertexAttributeType::Tangent, .item_size = 4});
    }

    if (layout.has_colors) {
        geometry->SetAttribute({.type = Geometry::VertexAttributeType::Color, .item_size = 3});
    }

    // Use the material from the first face. Shapes with mixed
    // materials (usemtl mid-group) are not currently supported.
    auto material_idx = mesh.material_ids.empty() ? -1 : mesh.material_ids.front();

    return {std::move(name), std::move(geometry), material_idx};
}

} // unnamed namespace

auto import(const fs::path& path) -> std::expected<OBJResult, std::string> {
    auto reader_config = tinyobj::ObjReaderConfig {};
    auto reader = tinyobj::ObjReader {};

    if (!reader.ParseFromFile(path.string(), reader_config)) {
        return reader.Error().empty()
            ? std::unexpected(std::format("Failed to load OBJ file {}", path.string()))
            : std::unexpected(std::format("OBJ parse error {}", reader.Error()));
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& tinyobj_materials = reader.GetMaterials();

    auto result = OBJResult {};

    result.materials.reserve(tinyobj_materials.size());
    for (const auto& material : tinyobj_materials) {
        result.materials.emplace_back(parse_material(material));
    }

    result.entries.reserve(shapes.size());
    for (const auto& shape : shapes) {
        result.entries.emplace_back(parse_shape(shape, attrib));
    }

    return result;
}

}
