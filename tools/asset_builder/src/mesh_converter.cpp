/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#define TINYOBJLOADER_IMPLEMENTATION

#include "vglx/asset_format.hpp"

#include "mesh_converter.hpp"
#include "texture_converter.hpp"

#include <array>
#include <cassert>
#include <cinttypes>
#include <cmath>
#include <filesystem>
#include <optional>
#include <print>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "tiny_obj_loader.hpp"

namespace fs = std::filesystem;

namespace {

constexpr auto eps = 1e-8f;

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

struct ShapeVertexLayout {
    uint32_t stride {0};
    uint32_t position_offset {0};
    uint32_t normal_offset {0};

    std::optional<uint32_t> uv_offset;
    std::optional<uint32_t> color_offset;
    std::optional<uint32_t> tangent_offset;

    bool has_uvs {false};
    bool has_tangents {false};
    bool has_colors {false};
};

auto make_layout(bool has_uvs, bool has_colors) {
    auto output = ShapeVertexLayout {};
    auto offset = uint32_t {0};

    output.has_uvs = has_uvs;
    output.has_tangents = has_uvs;
    output.has_colors = has_colors;

    output.position_offset = offset;
    offset += 3;
    output.normal_offset = offset;
    offset += 3;

    if (output.has_uvs) {
        output.uv_offset = offset;
        offset += 2;
    }
    if (output.has_tangents) {
        output.tangent_offset = offset;
        offset += 4;
    }
    if (output.has_colors) {
        output.color_offset = offset;
        offset += 3;
    }

    output.stride = offset;
    return output;
}

struct __vec2_t {
    float u {0};
    float v {0};

    [[nodiscard]] friend auto operator-(const __vec2_t a, const __vec2_t b) {
        return __vec2_t {a.u - b.u, a.v - b.v};
    }
};

struct __vec3_t {
    float x {0};
    float y {0};
    float z {0};

    auto& operator+=(const __vec3_t& rhs) {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
        return *this;
    }

    [[nodiscard]] friend auto operator*(float n, const __vec3_t v) {
        return __vec3_t {v.x * n, v.y * n, v.z * n};
    }

    [[nodiscard]] friend auto operator-(const __vec3_t a, const __vec3_t b) {
        return __vec3_t {a.x - b.x, a.y - b.y, a.z - b.z};
    }

    [[nodiscard]] auto Length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    auto Normalize() {
        const auto len = Length();
        if (len != 0.0f) {
            auto m = (1.0f / len);
            x *= m;
            y *= m;
            z *= m;
        }
        return *this;
    }
};

[[nodiscard]] auto cross(const __vec3_t a, const __vec3_t b) {
    return __vec3_t {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

[[nodiscard]] auto dot(const __vec3_t a, const __vec3_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

template<size_t N>
auto copy_fixed_size_str(char (&dst)[N], const std::string_view src) {
    std::memset(dst, 0, N);
    std::memcpy(dst, src.data(), std::min(src.size(), N - 1));
}

auto generate_normals(
    std::vector<float>& vertex_data,
    std::vector<unsigned>& index_data,
    const ShapeVertexLayout& layout
) {
    auto pos_offset = layout.position_offset;
    auto norm_offset = layout.normal_offset;

    for (size_t i = 0; i < index_data.size(); i += 3) {
        auto i0 = index_data[i + 0];
        auto i1 = index_data[i + 1];
        auto i2 = index_data[i + 2];

        auto v0 = __vec3_t {
            vertex_data[i0 * layout.stride + pos_offset + 0],
            vertex_data[i0 * layout.stride + pos_offset + 1],
            vertex_data[i0 * layout.stride + pos_offset + 2]
        };

        auto v1 = __vec3_t {
            vertex_data[i1 * layout.stride + pos_offset + 0],
            vertex_data[i1 * layout.stride + pos_offset + 1],
            vertex_data[i1 * layout.stride + pos_offset + 2],
        };

        auto v2 = __vec3_t {
            vertex_data[i2 * layout.stride + pos_offset + 0],
            vertex_data[i2 * layout.stride + pos_offset + 1],
            vertex_data[i2 * layout.stride + pos_offset + 2],
        };

        auto e0 = v1 - v0;
        auto e1 = v2 - v0;
        auto f = cross(e0, e1);
        if (dot(f, f) <= eps * eps) {
            // degenerate triangle → skip this face
            continue;
        }

        for (auto idx : {i0, i1, i2}) {
            vertex_data[idx * layout.stride + norm_offset + 0] += f.x;
            vertex_data[idx * layout.stride + norm_offset + 1] += f.y;
            vertex_data[idx * layout.stride + norm_offset + 2] += f.z;
        }
    }

    auto vertex_count = vertex_data.size() / layout.stride;
    for (size_t i = 0; i < vertex_count; ++i) {
        auto n = __vec3_t {
            vertex_data[i * layout.stride + norm_offset + 0],
            vertex_data[i * layout.stride + norm_offset + 1],
            vertex_data[i * layout.stride + norm_offset + 2]
        };

        if (n.Length() > 0.0f) {
            n.Normalize();
            vertex_data[i * layout.stride + norm_offset + 0] = n.x;
            vertex_data[i * layout.stride + norm_offset + 1] = n.y;
            vertex_data[i * layout.stride + norm_offset + 2] = n.z;
        }
    }
}

auto generate_tangents(
    std::vector<float>& vertex_data,
    std::vector<unsigned>& index_data,
    const ShapeVertexLayout& layout
) {
    assert(layout.has_uvs && layout.has_tangents);
    assert(layout.uv_offset && layout.tangent_offset);

    auto pos_offset = layout.position_offset;
    auto norm_offset = layout.normal_offset;
    auto uv_offset = layout.uv_offset.value();
    auto tan_offset = layout.tangent_offset.value();

    auto vertex_count = vertex_data.size() / layout.stride;
    std::vector<__vec3_t> t_accum(vertex_count);
    std::vector<__vec3_t> b_accum(vertex_count);

    for (size_t i = 0; i < index_data.size(); i += 3) {
        auto i0 = index_data[i + 0];
        auto i1 = index_data[i + 1];
        auto i2 = index_data[i + 2];

        auto v0 = __vec3_t {
            vertex_data[i0 * layout.stride + pos_offset + 0],
            vertex_data[i0 * layout.stride + pos_offset + 1],
            vertex_data[i0 * layout.stride + pos_offset + 2]
        };

        auto v1 = __vec3_t {
            vertex_data[i1 * layout.stride + pos_offset + 0],
            vertex_data[i1 * layout.stride + pos_offset + 1],
            vertex_data[i1 * layout.stride + pos_offset + 2],
        };

        auto v2 = __vec3_t {
            vertex_data[i2 * layout.stride + pos_offset + 0],
            vertex_data[i2 * layout.stride + pos_offset + 1],
            vertex_data[i2 * layout.stride + pos_offset + 2],
        };

        auto w0 = __vec2_t {
            vertex_data[i0 * layout.stride + uv_offset + 0],
            vertex_data[i0 * layout.stride + uv_offset + 1]
        };

        auto w1 = __vec2_t {
            vertex_data[i1 * layout.stride + uv_offset + 0],
            vertex_data[i1 * layout.stride + uv_offset + 1]
        };

        auto w2 = __vec2_t {
            vertex_data[i2 * layout.stride + uv_offset + 0],
            vertex_data[i2 * layout.stride + uv_offset + 1]
        };

        auto e0 = v1 - v0;
        auto e1 = v2 - v0;
        auto uv0 = w1 - w0;
        auto uv1 = w2 - w0;

        auto f = cross(e0, e1);
        if (dot(f, f) <= eps * eps) {
            // degenerate triangle → skip this face
            continue;
        }

        auto det = (uv0.u * uv1.v - uv1.u * uv0.v);
        if (std::fabs(det) < eps) {
            // degenerate UV triangle → skip this face
            continue;
        }
        auto r = 1.0f / det;

        const auto tangent = __vec3_t {
            (e0.x * uv1.v - e1.x * uv0.v) * r,
            (e0.y * uv1.v - e1.y * uv0.v) * r,
            (e0.z * uv1.v - e1.z * uv0.v) * r
        };

        const auto bitangent = __vec3_t {
            (e1.x * uv0.u - e0.x * uv1.u) * r,
            (e1.y * uv0.u - e0.y * uv1.u) * r,
            (e1.z * uv0.u - e0.z * uv1.u) * r
        };

        t_accum[i0] += tangent;
        t_accum[i1] += tangent;
        t_accum[i2] += tangent;

        b_accum[i0] += bitangent;
        b_accum[i1] += bitangent;
        b_accum[i2] += bitangent;
    }

    for (size_t i = 0; i < vertex_count; ++i) {
        auto n = __vec3_t {
            vertex_data[i * layout.stride + norm_offset + 0],
            vertex_data[i * layout.stride + norm_offset + 1],
            vertex_data[i * layout.stride + norm_offset + 2]
        };

        auto t = t_accum[i];
        auto b = b_accum[i];

        t = (t - dot(n, t) * n).Normalize();
        auto s = dot(cross(n, t), b) >= 0 ? 1.0f : -1.0f;

        vertex_data[i * layout.stride + tan_offset + 0] = t.x;
        vertex_data[i * layout.stride + tan_offset + 1] = t.y;
        vertex_data[i * layout.stride + tan_offset + 2] = t.z;
        vertex_data[i * layout.stride + tan_offset + 3] = s;
    }
}

auto convert_texture(
    const std::string& tex_name,
    MaterialTextureMapType tex_type,
    const fs::path& input_path,
    const fs::path& output_path
) -> std::expected<std::string, std::string> {
    auto tex_rel_path = fs::path {tex_name};
    auto tex_input_path = tex_rel_path;
    if (!tex_input_path.is_absolute()) {
        tex_input_path = input_path.parent_path() / tex_rel_path;
    }

    if (!fs::exists(tex_input_path)) {
        return std::unexpected("Failed to load texture " + tex_input_path.string());
    }

    auto tex_output_path = output_path.parent_path() / tex_rel_path;
    auto color_space = tex_type == MaterialTextureMapType_Diffuse
        ? TextureColorSpace_sRGB
        : TextureColorSpace_Linear;

    tex_output_path.replace_extension(".tex");
    if (auto result = ::convert_texture(tex_input_path, tex_output_path, color_space); !result) {
        return std::unexpected(result.error());
    }

    std::println("Generated texture {}", tex_output_path.string());
    return tex_rel_path.replace_extension(".tex").string();
}

auto parse_texture(
    const std::string& tex_name,
    MaterialTextureMapType tex_type,
    const fs::path& input_path,
    const fs::path& output_path
) -> std::expected<MaterialTextureMapRecord, std::string> {
    auto tex_converted_path = convert_texture(tex_name, tex_type, input_path, output_path);
    if (!tex_converted_path) {
        return std::unexpected(tex_converted_path.error());
    }

    auto texture_record = MaterialTextureMapRecord {};
    copy_fixed_size_str(texture_record.filename, tex_converted_path.value());
    texture_record.type = tex_type;
    return texture_record;
}

auto parse_materials(
    const std::vector<tinyobj::material_t> &materials,
    const fs::path& input_path,
    const fs::path& output_path,
    std::ofstream& out_stream
) {
    for (const auto& material : materials) {
        auto material_record = MaterialRecord {};
        copy_fixed_size_str(
            material_record.name,
            material.name.empty() ? "default:Material" : material.name
        );

        std::memcpy(material_record.ambient, material.ambient, sizeof(material_record.ambient));
        std::memcpy(material_record.diffuse, material.diffuse, sizeof(material_record.diffuse));

        // If a specular map exists but Ks is black, use a small default (0.1)
        // so highlights aren’t completely suppressed when the map drives specular.
        auto& specular = material.specular;
        const auto has_spec_map = !material.specular_texname.empty();
        const auto spec_color_zero = specular[0] == 0.0f;
        const auto& src = (has_spec_map && spec_color_zero)
            ? std::array {0.1f, 0.1f, 0.1f}
            : std::array {specular[0], specular[1], specular[2]};
        std::memcpy(material_record.specular, src.data(), sizeof(material_record.specular));
        material_record.shininess = material.shininess;

        material_record.texture_count = 0;
        auto texture_records = std::vector<MaterialTextureMapRecord> {};
        auto available_textures = std::array<std::pair<std::string, MaterialTextureMapType>, 4> {{
            {material.diffuse_texname, MaterialTextureMapType_Diffuse},
            {material.alpha_texname, MaterialTextureMapType_Alpha},
            {material.normal_texname, MaterialTextureMapType_Normal},
            {material.specular_texname, MaterialTextureMapType_Specular},
        }};

        for (const auto& [tex_name, tex_type] : available_textures) {
            if (!tex_name.empty()) {
                auto tex_record = parse_texture(tex_name, tex_type, input_path, output_path);
                if (tex_record) {
                    texture_records.emplace_back(tex_record.value());
                    material_record.texture_count++;
                } else {
                    std::println(stderr, "{}", tex_record.error());
                }
            }
        }

        out_stream.write(reinterpret_cast<const char*>(&material_record), sizeof(material_record));
        for (const auto& texture_record : texture_records) {
            out_stream.write(reinterpret_cast<const char*>(&texture_record), sizeof(texture_record));
        }
    }
}

auto parse_shapes(
    const std::vector<tinyobj::shape_t> &shapes,
    const tinyobj::attrib_t &attrib,
    std::ofstream& out_stream
) {
    for (const auto& shape : shapes) {
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

        for (auto i = 0u; i < mesh.indices.size(); ++i) {
            const auto idx = mesh.indices[i];
            VertexKey key = {idx.vertex_index, idx.texcoord_index};

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

        auto mesh_record = MeshRecord {};
        copy_fixed_size_str(
            mesh_record.name,
            shape.name.empty() ? "default:Mesh" : shape.name
        );

        mesh_record.vertex_count = static_cast<uint32_t>(seen_vertices.size());
        mesh_record.index_count = static_cast<uint32_t>(index_data.size());
        mesh_record.vertex_stride = layout.stride;
        mesh_record.material_index = mesh.material_ids.front();
        mesh_record.vertex_data_size = static_cast<uint64_t>(vertex_data.size() * sizeof(float));
        mesh_record.index_data_size = static_cast<uint64_t>(index_data.size() * sizeof(unsigned));
        mesh_record.vertex_flags = VertexAttr_HasPosition | VertexAttr_HasNormal;

        if (layout.has_uvs) mesh_record.vertex_flags |= VertexAttr_HasUV;
        if (layout.has_tangents) mesh_record.vertex_flags |= VertexAttr_HasTangent;
        if (layout.has_colors) mesh_record.vertex_flags |= VertexAttr_HasColor;

        out_stream.write(reinterpret_cast<const char*>(&mesh_record), sizeof(mesh_record));
        out_stream.write(reinterpret_cast<const char*>(vertex_data.data()), vertex_data.size() * sizeof(float));
        out_stream.write(reinterpret_cast<const char*>(index_data.data()), index_data.size() * sizeof(unsigned));
    }
}

} // unnamed namespace

auto convert_mesh(
    const fs::path& input_path,
    const fs::path& output_path
) -> std::expected<void, std::string> {
    auto reader_config = tinyobj::ObjReaderConfig {};
    auto reader = tinyobj::ObjReader {};

    if (!reader.ParseFromFile(input_path.string(), reader_config)) {
        return reader.Error().empty() ?
            std::unexpected("Error: Failed to load mesh " + input_path.string() + '\n') :
            std::unexpected("Error " + reader.Error());
    }

    if (!reader.Warning().empty()) {
        std::println("Warning: {}", reader.Warning());
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    auto header = MeshHeader {};
    std::memcpy(header.magic, "MSH0", 4);
    header.version = VGLX_MSH_VER;
    header.header_size = sizeof(MeshHeader);
    header.material_count = static_cast<uint32_t>(materials.size());
    header.mesh_count = static_cast<uint32_t>(shapes.size());

    if (output_path.has_parent_path()) {
        auto err = std::error_code {};
        fs::create_directories(output_path.parent_path(), err);
        if (err) return std::unexpected("Failed to create output directory");
    }

    auto out_stream = std::ofstream {output_path, std::ios::binary};
    if (!out_stream) {
        return std::unexpected("Failed to open output file: " + output_path.string());
    }

    out_stream.write(reinterpret_cast<const char*>(&header), sizeof(header));

    parse_materials(materials, input_path, output_path, out_stream);
    parse_shapes(shapes, attrib, out_stream);

    return {};
}