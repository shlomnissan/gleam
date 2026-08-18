/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/loaders.hpp"

#include "vglx/materials/pbr_material.hpp"
#include "vglx/materials/phong_material.hpp"
#include "vglx/math/quaternion.hpp"
#include "vglx/scene/mesh.hpp"
#include "vglx/scene/node.hpp"
#include "vglx/textures/texture_2d.hpp"

#include "loaders/detail/gltf_import.hpp"
#include "loaders/detail/image_import.hpp"
#include "loaders/detail/obj_import.hpp"
#include "loaders/detail/shared.hpp"

#include "utilities/logger.hpp"
#include "utilities/thread_pool.hpp"

#include <algorithm>
#include <vector>

namespace vglx {

namespace {

using ImageRef = std::pair<std::string, std::shared_ptr<Image>>;

auto load_texture(
    const detail::TextureRef& ref,
    const fs::path& base_dir,
    Texture::ColorSpace color_space,
    std::vector<ImageRef>& images
) -> std::shared_ptr<Texture2D> {
    if (ref.empty()) return nullptr;

    auto path = base_dir / ref.uri;
    if (fs::is_directory(path)) {
        return nullptr;
    }

    auto it = std::find_if(
        images.begin(), images.end(), [&path](const auto& e) {
            return path.string() == e.first;
        }
    );

    if (it == images.end()) {
        Logger::Log(LogLevel::Error,
            "Failed to load texture {}", path.string()
        );
        return nullptr;
    }

    auto texture = Texture2D::Create(it->second);

    texture->color_space = color_space;
    texture->wrap_s = ref.wrap_s;
    texture->wrap_t = ref.wrap_t;
    texture->min_filter = ref.min_filter;
    texture->mag_filter = ref.mag_filter;
    texture->generate_mipamps =
        ref.min_filter == Texture::MinFilter::NearestMipmapNearest ||
        ref.min_filter == Texture::MinFilter::LinearMipmapNearest  ||
        ref.min_filter == Texture::MinFilter::NearestMipmapLinear  ||
        ref.min_filter == Texture::MinFilter::LinearMipmapLinear;

    texture->transform.SetScale(ref.uv_scale);
    texture->transform.SetPosition(ref.uv_offset);
    texture->transform.SetRotation(ref.uv_rotation);

    return texture;
}

auto load_images(std::vector<fs::path> uris) {
    std::sort(uris.begin(), uris.end());
    auto it = std::unique(uris.begin(), uris.end());
    uris.erase(it, uris.end());

    auto threads = ThreadPool {};
    auto futures = std::vector<std::future<std::shared_ptr<Image>>>{};
    futures.reserve(uris.size());

    for (const auto& uri : uris) {
        futures.emplace_back(threads.Submit([uri] {
            return LoadImage(uri).value_or(nullptr);
        }));
    }

    auto output = std::vector<ImageRef> {};
    for (std::size_t i = 0; i < uris.size(); ++i) {
        if (auto image = futures[i].get()) {
            output.emplace_back(uris[i].string(), image);
        }
    }

    return output;
}

auto load_obj_material(
    const detail::obj::PhongMaterialDescriptor& desc,
    const fs::path& dir,
    std::vector<ImageRef>& images
) {
    using enum Texture::ColorSpace;

    auto material = PhongMaterial::Create();

    material->color = desc.diffuse;
    material->specular_color = desc.specular;
    material->emissive_color = desc.emission;
    material->shininess = desc.shininess;
    material->albedo_map = load_texture(desc.tex_diffuse, dir, sRGB, images);
    material->alpha_map = load_texture(desc.tex_alpha, dir, Linear, images);
    material->normal_map = load_texture(desc.tex_normal, dir, Linear, images);
    material->specular_map = load_texture(desc.tex_specular, dir, Linear, images);
    material->emissive_map = load_texture(desc.tex_emissive, dir, sRGB, images);

    if (material->albedo_map) {
        material->color = 0xFFFFFFu;
    }

    return material;
}

auto load_obj_images(
    const std::vector<detail::obj::PhongMaterialDescriptor>& materials,
    const fs::path& base_dir
) {
    auto uris = std::vector<fs::path> {};

    for (const auto& desc : materials) {
        if (!desc.tex_diffuse.empty()) uris.emplace_back(base_dir / desc.tex_diffuse.uri);
        if (!desc.tex_alpha.empty()) uris.emplace_back(base_dir / desc.tex_alpha.uri);
        if (!desc.tex_normal.empty()) uris.emplace_back(base_dir / desc.tex_normal.uri);
        if (!desc.tex_specular.empty()) uris.emplace_back(base_dir / desc.tex_specular.uri);
        if (!desc.tex_emissive.empty()) uris.emplace_back(base_dir / desc.tex_emissive.uri);
    }

    return load_images(uris);
}

auto load_obj_mesh(const fs::path& path) -> std::expected<std::unique_ptr<Node>, std::string> {
    auto result = detail::obj::import(path);
    if (!result) {
        return std::unexpected(result.error());
    }

    auto base_dir = path.parent_path();
    auto obj = result.value();

    auto images = load_obj_images(obj.materials, base_dir);
    auto materials = std::vector<std::shared_ptr<PhongMaterial>> {};
    materials.reserve(obj.materials.size());
    for (const auto& desc : obj.materials) {
        materials.emplace_back(load_obj_material(desc, base_dir, images));
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

auto load_gltf_material(
    const detail::gltf::PBRMaterialDescriptor& desc,
    const fs::path& dir,
    std::vector<ImageRef>& images
) {
    using enum Texture::ColorSpace;

    return PBRMaterial::Create({
        .color = desc.base_color,
        .emissive_color = desc.emissive,
        .ao_intensity = desc.ao_intensity,
        .emissive_intensity = desc.emissive_intensity,
        .metallic = desc.metallic,
        .normal_intensity = desc.normal_intensity,
        .roughness = desc.roughness,
        .ao_map = load_texture(desc.tex_occlusion, dir, Linear, images),
        .albedo_map = load_texture(desc.tex_base_color, dir, sRGB, images),
        .emissive_map = load_texture(desc.tex_emissive, dir, sRGB, images),
        .metallic_map = load_texture(desc.tex_metallic_roughness, dir, Linear, images),
        .normal_map = load_texture(desc.tex_normal, dir, Linear, images),
        .roughness_map = load_texture(desc.tex_metallic_roughness, dir, Linear, images),
    });
}

auto load_gltf_images(
    const std::vector<detail::gltf::PBRMaterialDescriptor>& materials,
    const fs::path& base_dir
) {
    auto uris = std::vector<fs::path> {};

    for (const auto& desc : materials) {
        if (!desc.tex_occlusion.empty()) uris.emplace_back(base_dir / desc.tex_occlusion.uri);
        if (!desc.tex_base_color.empty()) uris.emplace_back(base_dir / desc.tex_base_color.uri);
        if (!desc.tex_emissive.empty()) uris.emplace_back(base_dir / desc.tex_emissive.uri);
        if (!desc.tex_metallic_roughness.empty()) uris.emplace_back(base_dir / desc.tex_metallic_roughness.uri);
        if (!desc.tex_normal.empty()) uris.emplace_back(base_dir / desc.tex_normal.uri);
    }

    return load_images(uris);
}

auto load_gltf_mesh(const fs::path& path) -> std::expected<std::unique_ptr<Node>, std::string> {
    auto result = detail::gltf::import(path);
    if (!result) {
        return std::unexpected(result.error());
    }

    auto& gltf = result.value();
    auto base_dir = path.parent_path();

    auto images = load_gltf_images(gltf.materials, base_dir);
    auto materials = std::vector<std::shared_ptr<PBRMaterial>> {};
    materials.reserve(gltf.materials.size());

    for (const auto& desc : gltf.materials) {
        materials.emplace_back(load_gltf_material(desc, base_dir, images));
    }

    auto nodes_count = static_cast<int>(gltf.nodes.size());
    auto nodes = std::vector<std::unique_ptr<Node>>(nodes_count);
    auto node_ptrs = std::vector<Node*>(nodes_count);

    for (auto i = 0; i < nodes_count; ++i) {
        nodes[i] = Node::Create();
        node_ptrs[i] = nodes[i].get();
    }

    for (auto i = 0; i < nodes_count; ++i) {
        const auto& entry = gltf.nodes[i];
        auto* node = node_ptrs[i];

        if (!entry.name.empty()) {
            node->SetName(entry.name);
        }

        node->transform.SetPosition(entry.position);
        node->transform.SetScale(entry.scale);
        node->transform.SetRotation(entry.rotation);

        for (const auto& primitive : entry.primitives) {
            auto has_material =
                primitive.material_index >= 0 &&
                primitive.material_index < static_cast<int>(materials.size());

            auto material = has_material
                ? materials[primitive.material_index]
                : PBRMaterial::Create();

            node->Add(Mesh::Create(primitive.geometry, material));
        }

        for (const auto child_idx :entry.children) {
            node_ptrs[i]->Add(std::move(nodes[child_idx]));
        }
    }

    auto root = std::make_unique<Node>();
    for (const auto root_idx : gltf.roots) {
        root->Add(std::move(nodes[root_idx]));
    }

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
