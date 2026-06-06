/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#define CGLTF_IMPLEMENTATION

#include "loaders/detail/gltf_import.hpp"
#include "utilities/assert.hpp"
#include "utilities/logger.hpp"

#include "misc/cgltf.hpp"

namespace vglx::detail::gltf {

namespace {

auto create_geometry(cgltf_primitive* primitive) -> std::shared_ptr<vglx::Geometry> {
    if (primitive->type != cgltf_primitive_type_triangles) {
        Logger::Log(
            LogLevel::Warning,
            "glTF import only supports triangle primitives"
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
            "glTF import skipped primitive without vertex positions"
        );
        return nullptr;
    }

    const auto has_normals = norm_ptr != nullptr;
    const auto has_texcoords = tex_ptr != nullptr;
    const auto has_tangents = tan_ptr != nullptr;
    const auto has_colors = color_ptr != nullptr;

    const auto layout = make_layout(has_texcoords, has_colors);
    const auto vertex_count = pos_ptr->count;

    auto vertex_data = std::vector<float>(vertex_count * layout.stride);

    if (has_colors && color_ptr->type == cgltf_type_vec4) {
        Logger::Log(
            LogLevel::Warning,
            "glTF import only supports RGB vertex colors (no alpha)"
        );
    }

    float fl2[2], fl3[3], fl4[4];

    for (cgltf_size i = 0; i < vertex_count; ++i) {
        cgltf_accessor_read_float(pos_ptr, i, fl3, 3);

        const auto base = i * layout.stride;

        vertex_data[base + layout.position_offset + 0] = fl3[0];
        vertex_data[base + layout.position_offset + 1] = fl3[1];
        vertex_data[base + layout.position_offset + 2] = fl3[2];

        if (has_normals) {
            const auto offset = base + layout.normal_offset;
            cgltf_accessor_read_float(norm_ptr, i, fl3, 3);

            vertex_data[offset + 0] = fl3[0];
            vertex_data[offset + 1] = fl3[1];
            vertex_data[offset + 2] = fl3[2];
        }

        if (layout.has_uvs) {
            const auto offset = base + layout.uv_offset.value();
            cgltf_accessor_read_float(tex_ptr, i, fl2, 2);

            vertex_data[offset + 0] = fl2[0];
            vertex_data[offset + 1] = fl2[1];
        }

        if (layout.has_tangents && has_tangents) {
            const auto offset = base + layout.tangent_offset.value();
            cgltf_accessor_read_float(tan_ptr, i, fl4, 4);

            vertex_data[offset + 0] = fl4[0];
            vertex_data[offset + 1] = fl4[1];
            vertex_data[offset + 2] = fl4[2];
            vertex_data[offset + 3] = fl4[3];
        }

        if (layout.has_colors) {
            const auto offset = base + layout.color_offset.value();
            cgltf_accessor_read_float(color_ptr, i, fl4, 4);

            vertex_data[offset + 0] = fl4[0];
            vertex_data[offset + 1] = fl4[1];
            vertex_data[offset + 2] = fl4[2];
        }
    }

    auto index_data = std::vector<unsigned> {};
    if (primitive->indices) {
        auto index_accessor = primitive->indices;
        index_data.resize(index_accessor->count);

        for (cgltf_size i = 0; i < index_accessor->count; i++) {
            index_data[i] = cgltf_accessor_read_index(index_accessor, i);
        }
    } else {
        index_data.resize(vertex_count);
        for (unsigned i = 0; i < vertex_count; ++i) {
            index_data[i] = i;
        }
    }

    if (!has_normals) {
        generate_normals(vertex_data, index_data, layout);
    }

    if (layout.has_tangents && !has_tangents) {
        generate_tangents(vertex_data, index_data, layout);
    }

    auto geometry = Geometry::Create(vertex_data, index_data);
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

    return geometry;
}

auto parse_primitives(const cgltf_data* data) {
    auto output = std::vector<std::vector<GLTFPrimitive>>(data->meshes_count);

    for (cgltf_size i = 0; i < data->meshes_count; ++i) {
        auto mesh = &data->meshes[i];
        for (cgltf_size j = 0; j < mesh->primitives_count; ++j) {
            auto primitive = &mesh->primitives[j];
            if (auto geometry = create_geometry(primitive)) {
                output[i].emplace_back(GLTFPrimitive {
                    .geometry = std::move(geometry),
                    .material_index = primitive->material
                        ? static_cast<int>(cgltf_material_index(data, primitive->material))
                        : -1,
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

    if (view->texture->image && view->texture->image->uri) {
        desc.uri = view->texture->image->uri;
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
                "glTF import doesn't support PBR specular glossiness"
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

}

auto import(const fs::path& path) -> std::expected<GLTFResult, std::string> {
    auto options = cgltf_options {};

    cgltf_data* data = nullptr;

    if (cgltf_parse_file(&options, path.string().c_str(), &data) != cgltf_result_success) {
        return std::unexpected("Error: Failed to parse glTF file structure");
    }

    auto guard = std::unique_ptr<cgltf_data, decltype(&cgltf_free)>{data, cgltf_free};

    if (cgltf_load_buffers(&options, data, path.string().c_str()) != cgltf_result_success) {
        return std::unexpected("Error: Failed to load glTF buffers");
    }

    if (cgltf_validate(data) != cgltf_result_success) {
        return std::unexpected("Error: glTF validation failed, file may be malformed");
    }

    auto primitives = parse_primitives(data);
    auto materials = parse_materials(data);

    return std::unexpected("implementing...");
}

}
