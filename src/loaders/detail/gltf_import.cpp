/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#define CGLTF_IMPLEMENTATION

#include "loaders/detail/gltf_import.hpp"
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

auto parse_materials(const cgltf_data* data) {
    auto output = std::vector<PBRMaterialDescriptor> {};

    for (cgltf_size i = 0; i < data->materials_count; ++i) {
        auto material = &data->materials[i];
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
