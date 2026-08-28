/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#define CGLTF_IMPLEMENTATION

#include "loaders/detail/gltf_import.hpp"

#include "vglx/math/matrix4.hpp"
#include "vglx/textures/texture.hpp"

#include "geometries/detail/generate_attributes.hpp"
#include "geometries/vertex_streams.hpp"
#include "misc/cgltf.hpp"
#include "utilities/assert.hpp"
#include "utilities/logger.hpp"

#include <cstdint>
#include <utility>

namespace vglx::detail::gltf {

namespace {

auto create_geometry(cgltf_primitive* primitive) -> std::shared_ptr<vglx::Geometry> {
    if (primitive->type != cgltf_primitive_type_triangles) {
        Logger::Log(
            LogLevel::Warning,
            "glTF only supports triangle primitives"
        );
        return nullptr;
    }
    const auto* pos_ptr = cgltf_find_accessor(primitive, cgltf_attribute_type_position, 0);
    const auto* norm_ptr = cgltf_find_accessor(primitive, cgltf_attribute_type_normal, 0);
    const auto* tex_ptr = cgltf_find_accessor(primitive, cgltf_attribute_type_texcoord, 0);
    const auto* tan_ptr = cgltf_find_accessor(primitive, cgltf_attribute_type_tangent, 0);
    const auto* color_ptr = cgltf_find_accessor(primitive, cgltf_attribute_type_color, 0);

    if (pos_ptr == nullptr) {
        Logger::Log(
            LogLevel::Warning,
            "glTF skipped primitive without vertex positions"
        );
        return nullptr;
    }

    const auto has_normals = norm_ptr != nullptr;
    const auto has_uvs = tex_ptr != nullptr;
    const auto has_tangents = has_normals && tan_ptr != nullptr;
    const auto has_colors = color_ptr != nullptr;

    const auto vertex_count = pos_ptr->count;

    auto streams = VertexStreams {};
    streams.positions.resize(vertex_count * 3);

    if (has_normals) streams.normals.resize(vertex_count * 3);
    if (has_uvs) streams.uvs.resize(vertex_count * 2);
    if (has_uvs && has_tangents) streams.tangents.resize(vertex_count * 4);
    if (has_colors) streams.colors.resize(vertex_count * 3);

    if (has_colors && color_ptr->type == cgltf_type_vec4) {
        Logger::Log(
            LogLevel::Warning,
            "glTF import only supports RGB vertex colors (no alpha)"
        );
    }

    float fl2[2], fl3[3], fl4[4];

    for (cgltf_size i = 0; i < vertex_count; ++i) {
        cgltf_accessor_read_float(pos_ptr, i, fl3, 3);

        streams.positions[i * 3 + 0] = fl3[0];
        streams.positions[i * 3 + 1] = fl3[1];
        streams.positions[i * 3 + 2] = fl3[2];

        if (has_normals) {
            cgltf_accessor_read_float(norm_ptr, i, fl3, 3);

            streams.normals[i * 3 + 0] = fl3[0];
            streams.normals[i * 3 + 1] = fl3[1];
            streams.normals[i * 3 + 2] = fl3[2];
        }

        if (has_uvs) {
            cgltf_accessor_read_float(tex_ptr, i, fl2, 2);

            streams.uvs[i * 2 + 0] = fl2[0];
            streams.uvs[i * 2 + 1] = 1.0f - fl2[1];
        }

        if (has_uvs && has_tangents) {
            cgltf_accessor_read_float(tan_ptr, i, fl4, 4);

            streams.tangents[i * 4 + 0] = fl4[0];
            streams.tangents[i * 4 + 1] = fl4[1];
            streams.tangents[i * 4 + 2] = fl4[2];
            streams.tangents[i * 4 + 3] = fl4[3];
        }

        if (has_colors) {
            cgltf_accessor_read_float(color_ptr, i, fl4, 4);

            streams.colors[i * 3 + 0] = fl4[0];
            streams.colors[i * 3 + 1] = fl4[1];
            streams.colors[i * 3 + 2] = fl4[2];
        }
    }

    if (primitive->indices) {
        auto index_accessor = primitive->indices;
        streams.indices.resize(index_accessor->count);

        for (cgltf_size i = 0; i < index_accessor->count; i++) {
            streams.indices[i] = cgltf_accessor_read_index(index_accessor, i);
        }
    } else {
        streams.indices.resize(vertex_count);
        for (uint32_t i = 0; i < vertex_count; ++i) {
            streams.indices[i] = i;
        }
    }

    if (has_normals && has_uvs && !has_tangents) {
        streams.tangents = generate_tangents(
            streams.positions,
            streams.normals,
            streams.uvs,
            streams.indices
        );
    }

    auto geometry = Geometry::Create();
    streams.AddTo(*geometry);

    return geometry;
}

auto parse_primitives(const cgltf_data* data) {
    auto output = std::vector<std::vector<GLTFPrimitive>>(data->meshes_count);

    for (cgltf_size i = 0; i < data->meshes_count; ++i) {
        auto mesh = &data->meshes[i];
        for (cgltf_size j = 0; j < mesh->primitives_count; ++j) {
            auto primitive = &mesh->primitives[j];
            auto has_normals = cgltf_find_accessor(primitive, cgltf_attribute_type_normal, 0) != nullptr;
            auto index = primitive->material ? int(cgltf_material_index(data, primitive->material)) : -1;
            if (auto geometry = create_geometry(primitive)) {
                output[i].emplace_back(GLTFPrimitive {
                    .geometry = std::move(geometry),
                    .material_index = index,
                    .flat_shaded = !has_normals,
                });
            }
        }
    }

    return output;
}

auto get_min_filter(cgltf_filter_type type) {
    switch(type) {
        case cgltf_filter_type_nearest:
            return Texture::MinFilter::Nearest;
        case cgltf_filter_type_linear:
        case cgltf_filter_type_undefined:
            return Texture::MinFilter::Linear;
        case cgltf_filter_type_nearest_mipmap_nearest:
            return Texture::MinFilter::NearestMipmapNearest;
        case cgltf_filter_type_linear_mipmap_nearest:
            return Texture::MinFilter::LinearMipmapNearest;
        case cgltf_filter_type_nearest_mipmap_linear:
            return Texture::MinFilter::NearestMipmapLinear;
        case cgltf_filter_type_linear_mipmap_linear:
            return Texture::MinFilter::LinearMipmapLinear;
        default: VGLX_UNREACHABLE();
    }
}

auto get_mag_filter(cgltf_filter_type type) {
    return type == cgltf_filter_type_nearest
        ? Texture::MagFilter::Nearest
        : Texture::MagFilter::Linear;
}

auto get_wrap_mode(cgltf_wrap_mode mode) {
    switch(mode) {
        case cgltf_wrap_mode_clamp_to_edge:
            return Texture::Wrapping::ClampToEdge;
        case cgltf_wrap_mode_mirrored_repeat:
            return Texture::Wrapping::MirroredRepeat;
        case cgltf_wrap_mode_repeat:
            return Texture::Wrapping::Repeat;
        default: VGLX_UNREACHABLE();
    }
}

auto parse_texture(const cgltf_texture_view* view) {
    auto desc = TextureRef {};

    if (view->texture == nullptr) return desc;

    if (view->texture->image) {
        if (view->texture->image->uri) {
            desc.uri = view->texture->image->uri;
        } else {
            Logger::Log(LogLevel::Warning, "glTF embedded textures are not supported");
        }
    }

    if (const auto sampler = view->texture->sampler) {
        desc.min_filter = get_min_filter(sampler->min_filter);
        desc.mag_filter = get_mag_filter(sampler->mag_filter);
        desc.wrap_s = get_wrap_mode(sampler->wrap_s);
        desc.wrap_t = get_wrap_mode(sampler->wrap_t);
    }

    if (view->has_transform) {
        desc.uv_offset = {view->transform.offset[0], view->transform.offset[1]};
        desc.uv_scale = {view->transform.scale[0],  view->transform.scale[1]};
        desc.uv_rotation = view->transform.rotation;
    }

    return desc;
}

auto parse_materials(const cgltf_data* data) {
    auto output = std::vector<PBRMaterialDescriptor> {};
    output.reserve(data->materials_count);

    for (cgltf_size i = 0; i < data->materials_count; ++i) {
        const auto* mat = &data->materials[i];
        auto desc = PBRMaterialDescriptor {};

        if (mat->name) desc.name = mat->name;

        if (mat->has_pbr_specular_glossiness) {
            Logger::Log(
                LogLevel::Warning,
                "glTF doesn't support PBR specular glossiness"
            );
        }

        if (mat->has_pbr_metallic_roughness) {
            const auto& pbr = mat->pbr_metallic_roughness;

            desc.metallic = pbr.metallic_factor;
            desc.roughness = pbr.roughness_factor;
            desc.base_color = {
                pbr.base_color_factor[0],
                pbr.base_color_factor[1],
                pbr.base_color_factor[2]
            };

            if (pbr.base_color_texture.texture) {
                desc.tex_base_color = parse_texture(&pbr.base_color_texture);
            }

            if (pbr.metallic_roughness_texture.texture) {
                desc.tex_metallic_roughness = parse_texture(&pbr.metallic_roughness_texture);
            }
        }

        desc.emissive = {
            mat->emissive_factor[0],
            mat->emissive_factor[1],
            mat->emissive_factor[2]
        };

        if (mat->has_emissive_strength) {
            desc.emissive_intensity = mat->emissive_strength.emissive_strength;
        }

        if (mat->normal_texture.texture) {
            desc.normal_intensity = mat->normal_texture.scale;
            desc.tex_normal = parse_texture(&mat->normal_texture);
        }

        if (mat->occlusion_texture.texture) {
            desc.ao_intensity = mat->occlusion_texture.scale;
            desc.tex_occlusion = parse_texture(&mat->occlusion_texture);
        }

        if (mat->emissive_texture.texture) {
            desc.tex_emissive = parse_texture(&mat->emissive_texture);
        }

        output.emplace_back(std::move(desc));
    }

    return output;
}

auto decompose_matrix (GLTFNodeEntry &entry, const cgltf_float* mat) {
    auto sx = Vector3 {mat[0], mat[1], mat[2]}.Length();
    auto sy = Vector3 {mat[4], mat[5], mat[6]}.Length();
    auto sz = Vector3 {mat[8], mat[9], mat[10]}.Length();

    const auto n0 = Vector3 {mat[0]/sx, mat[1]/sx, mat[2]/sx};
    const auto n1 = Vector3 {mat[4]/sy, mat[5]/sy, mat[6]/sy};
    const auto n2 = Vector3 {mat[8]/sz, mat[9]/sz, mat[10]/sz};

    if (Dot(Cross(n0, n1), n2) < 0.0f) sz = -sz;

    const auto rotation = Matrix4 {
        mat[0]/sx, mat[4]/sy, mat[8]/sz,  0.0f,
        mat[1]/sx, mat[5]/sy, mat[9]/sz,  0.0f,
        mat[2]/sx, mat[6]/sy, mat[10]/sz, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    entry.position = {mat[12], mat[13], mat[14]};
    entry.scale = {sx, sy, sz};
    entry.rotation = Quaternion {rotation};
}

auto parse_nodes(const cgltf_data* data) {
    auto output = std::vector<GLTFNodeEntry>(data->nodes_count);
    auto primitives = parse_primitives(data);

    for (cgltf_size i = 0; i < data->nodes_count; ++i) {
        const auto* node = &data->nodes[i];
        auto& entry = output[i];

        if (node->name) entry.name = node->name;

        if (node->has_matrix) {
            decompose_matrix (entry, node->matrix);
        } else {
            auto& t = node->translation;
            auto& r = node->rotation;
            auto& s = node->scale;

            entry.position = {t[0], t[1], t[2]};
            entry.rotation = {r[0], r[1], r[2], r[3]};
            entry.scale = {s[0], s[1], s[2]};
        }

        entry.children.reserve(node->children_count);
        for (cgltf_size j = 0; j < node->children_count; ++j) {
            entry.children.emplace_back(
                static_cast<int>(cgltf_node_index(data, node->children[j]))
            );
        }

        if (node->mesh) {
            entry.primitives = primitives[cgltf_mesh_index(data, node->mesh)];
        }
    }

    return output;
}

auto get_roots(const cgltf_data* data) {
    auto output = std::vector<int> {};
    output.reserve(data->scene->nodes_count);

    for (cgltf_size i = 0; i < data->scene->nodes_count; ++i) {
        output.emplace_back(
            static_cast<int>(cgltf_node_index(data, data->scene->nodes[i]))
        );
    }

    return output;
}

}

auto import(const fs::path& path) -> std::expected<GLTFResult, std::string> {
    auto options = cgltf_options {};

    cgltf_data* data = nullptr;

    if (cgltf_parse_file(&options, path.string().c_str(), &data) != cgltf_result_success) {
        return std::unexpected("glTF failed to parse file structure");
    }

    auto guard = std::unique_ptr<cgltf_data, decltype(&cgltf_free)>{data, cgltf_free};

    if (cgltf_load_buffers(&options, data, path.string().c_str()) != cgltf_result_success) {
        return std::unexpected("glTF failed to load buffers");
    }

    if (cgltf_validate(data) != cgltf_result_success) {
        return std::unexpected("glTF validation failed, file may be malformed");
    }

    if (data->scene == nullptr) {
        return std::unexpected("glTF file has no default scene");
    }

    return GLTFResult {
        .nodes = parse_nodes(data),
        .roots = get_roots(data),
        .materials = parse_materials(data)
    };
}

}
