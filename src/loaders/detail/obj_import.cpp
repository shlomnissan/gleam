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

#include <array>
#include <cassert>
#include <cmath>
#include <format>
#include <optional>
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

struct VertexLayout {
    uint32_t stride {0};
    uint32_t position_offset {0};
    uint32_t normal_offset {0};

    std::optional<uint32_t> uv_offset;
    std::optional<uint32_t> tangent_offset;
    std::optional<uint32_t> color_offset;

    bool has_uvs {false};
    bool has_tangents {false};
    bool has_colors {false};
};

auto make_layout(bool has_uvs, bool has_colors) -> VertexLayout {
    auto layout = VertexLayout {};
    auto offset = uint32_t {0};

    layout.has_uvs = has_uvs;
    layout.has_tangents = has_uvs;
    layout.has_colors = has_colors;

    layout.position_offset = offset;
    offset += 3;

    layout.normal_offset = offset;
    offset += 3;

    if (layout.has_uvs) {
        layout.uv_offset = offset;
        offset += 2;
    }

    if (layout.has_tangents) {
        layout.tangent_offset = offset;
        offset += 4;
    }

    if (layout.has_colors) {
        layout.color_offset = offset;
        offset += 3;
    }

    layout.stride = offset;

    return layout;
}

auto generate_normals(
    std::vector<float>& vertex_data,
    std::vector<unsigned>& index_data,
    const VertexLayout& layout
) {
    for (size_t i = 0; i < index_data.size(); i += 3) {
        auto i0 = index_data[i + 0];
        auto i1 = index_data[i + 1];
        auto i2 = index_data[i + 2];

        auto v0 = Vector3 {
            vertex_data[i0 * layout.stride + layout.position_offset + 0],
            vertex_data[i0 * layout.stride + layout.position_offset + 1],
            vertex_data[i0 * layout.stride + layout.position_offset + 2]
        };

        auto v1 = Vector3 {
            vertex_data[i1 * layout.stride + layout.position_offset + 0],
            vertex_data[i1 * layout.stride + layout.position_offset + 1],
            vertex_data[i1 * layout.stride + layout.position_offset + 2]
        };

        auto v2 = Vector3 {
            vertex_data[i2 * layout.stride + layout.position_offset + 0],
            vertex_data[i2 * layout.stride + layout.position_offset + 1],
            vertex_data[i2 * layout.stride + layout.position_offset + 2]
        };

        auto e0 = v1 - v0;
        auto e1 = v2 - v0;
        auto f = Cross(e0, e1);
        if (Dot(f, f) <= math::eps * math::eps) continue;

        for (auto idx : {i0, i1, i2}) {
            vertex_data[idx * layout.stride + layout.normal_offset + 0] += f.x;
            vertex_data[idx * layout.stride + layout.normal_offset + 1] += f.y;
            vertex_data[idx * layout.stride + layout.normal_offset + 2] += f.z;
        }
    }

    auto vertex_count = vertex_data.size() / layout.stride;
    for (size_t i = 0; i < vertex_count; ++i) {
        auto n = Vector3 {
            vertex_data[i * layout.stride + layout.normal_offset + 0],
            vertex_data[i * layout.stride + layout.normal_offset + 1],
            vertex_data[i * layout.stride + layout.normal_offset + 2]
        };

        if (n.Length() > 0.0f) {
            n.Normalize();
            vertex_data[i * layout.stride + layout.normal_offset + 0] = n.x;
            vertex_data[i * layout.stride + layout.normal_offset + 1] = n.y;
            vertex_data[i * layout.stride + layout.normal_offset + 2] = n.z;
        }
    }
}

auto generate_tangents(
    std::vector<float>& vertex_data,
    std::vector<unsigned>& index_data,
    const VertexLayout& layout
) {
    assert(layout.has_uvs && layout.has_tangents);
    assert(layout.uv_offset && layout.tangent_offset);

    auto uv_offset = layout.uv_offset.value();
    auto tan_offset = layout.tangent_offset.value();

    auto vertex_count = vertex_data.size() / layout.stride;
    auto t_accum = std::vector<Vector3>(vertex_count);
    auto b_accum = std::vector<Vector3>(vertex_count);

    for (size_t i = 0; i < index_data.size(); i += 3) {
        auto i0 = index_data[i + 0];
        auto i1 = index_data[i + 1];
        auto i2 = index_data[i + 2];

        auto v0 = Vector3 {
            vertex_data[i0 * layout.stride + layout.position_offset + 0],
            vertex_data[i0 * layout.stride + layout.position_offset + 1],
            vertex_data[i0 * layout.stride + layout.position_offset + 2]
        };

        auto v1 = Vector3 {
            vertex_data[i1 * layout.stride + layout.position_offset + 0],
            vertex_data[i1 * layout.stride + layout.position_offset + 1],
            vertex_data[i1 * layout.stride + layout.position_offset + 2]
        };

        auto v2 = Vector3 {
            vertex_data[i2 * layout.stride + layout.position_offset + 0],
            vertex_data[i2 * layout.stride + layout.position_offset + 1],
            vertex_data[i2 * layout.stride + layout.position_offset + 2]
        };

        auto w0 = Vector2 {
            vertex_data[i0 * layout.stride + uv_offset + 0],
            vertex_data[i0 * layout.stride + uv_offset + 1]
        };

        auto w1 = Vector2 {
            vertex_data[i1 * layout.stride + uv_offset + 0],
            vertex_data[i1 * layout.stride + uv_offset + 1]
        };

        auto w2 = Vector2 {
            vertex_data[i2 * layout.stride + uv_offset + 0],
            vertex_data[i2 * layout.stride + uv_offset + 1]
        };

        auto e0 = v1 - v0;
        auto e1 = v2 - v0;
        auto uv0 = w1 - w0;
        auto uv1 = w2 - w0;

        auto f = Cross(e0, e1);
        if (Dot(f, f) <= math::eps * math::eps) continue;

        auto det = (uv0.x * uv1.y - uv1.x * uv0.y);
        if (std::fabs(det) < math::eps) continue;
        auto r = 1.0f / det;

        auto tangent = Vector3 {
            (e0.x * uv1.y - e1.x * uv0.y) * r,
            (e0.y * uv1.y - e1.y * uv0.y) * r,
            (e0.z * uv1.y - e1.z * uv0.y) * r
        };

        auto bitangent = Vector3 {
            (e1.x * uv0.x - e0.x * uv1.x) * r,
            (e1.y * uv0.x - e0.y * uv1.x) * r,
            (e1.z * uv0.x - e0.z * uv1.x) * r
        };

        t_accum[i0] += tangent;
        t_accum[i1] += tangent;
        t_accum[i2] += tangent;

        b_accum[i0] += bitangent;
        b_accum[i1] += bitangent;
        b_accum[i2] += bitangent;
    }

    for (size_t i = 0; i < vertex_count; ++i) {
        auto n = Vector3 {
            vertex_data[i * layout.stride + layout.normal_offset + 0],
            vertex_data[i * layout.stride + layout.normal_offset + 1],
            vertex_data[i * layout.stride + layout.normal_offset + 2]
        };

        auto t = t_accum[i];
        auto b = b_accum[i];

        t = Normalize(t - Dot(n, t) * n);
        auto s = Dot(Cross(n, t), b) >= 0 ? 1.0f : -1.0f;

        vertex_data[i * layout.stride + tan_offset + 0] = t.x;
        vertex_data[i * layout.stride + tan_offset + 1] = t.y;
        vertex_data[i * layout.stride + tan_offset + 2] = t.z;
        vertex_data[i * layout.stride + tan_offset + 3] = s;
    }
}

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
