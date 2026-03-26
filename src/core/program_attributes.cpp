/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "core/program_attributes.hpp"

#include "vglx/materials/phong_material.hpp"
#include "vglx/materials/shader_material.hpp"
#include "vglx/materials/sprite_material.hpp"
#include "vglx/materials/unlit_material.hpp"
#include "vglx/math/utilities.hpp"

#include <cassert>

namespace vglx {

ProgramAttributes::ProgramAttributes(
    Renderable* renderable,
    const LightsCounter& lights,
    const Scene* scene
) {
    auto geometry = renderable->GetGeometry().get();
    auto material = renderable->GetMaterial().get();

    type = material->GetType();

    if (type == Material::Type::PhongMaterial) {
        auto m = static_cast<const PhongMaterial*>(material);
        color = true;
        albedo_map = m->albedo_map != nullptr;
        alpha_map = m->alpha_map != nullptr;
        normal_map = m->normal_map != nullptr;
        specular_map = m->specular_map != nullptr;
        emissive_map = m->emissive_map != nullptr;
    }

    auto shader_material_id = 0;
    if (type == Material::Type::ShaderMaterial) {
        auto m = static_cast<const ShaderMaterial*>(material);
        shader_material_id = m->shader_material_id_;
        vertex_shader = m->vertex_shader_;
        fragment_shader = m->fragment_shader_;
    }

    if (type == Material::Type::SpriteMaterial) {
        auto m = static_cast<const SpriteMaterial*>(material);
        color = true;
        texture_map = m->texture_map != nullptr;
        size_attenuation = m->size_attenuation;
    }

    if (type == Material::Type::UnlitMaterial) {
        auto m = static_cast<const UnlitMaterial*>(material);
        color = true;
        texture_map = m->texture_map != nullptr;
        alpha_map = m->alpha_map != nullptr;
    }

    flat_shaded = material->flat_shaded;
    fog = material->fog && scene->fog.has_value();
    instancing = renderable->GetNodeType() == Node::Type::InstancedMesh;
    num_lights = lights.directional + lights.point + lights.spot;
    two_sided = material->two_sided;
    vertex_color = geometry->HasAttribute(Geometry::VertexAttributeType::Color);
    tangent = geometry->HasAttribute(Geometry::VertexAttributeType::Tangent);

    static_assert(std::to_underlying(Material::Type::Length) <= 15);

    key |= (std::to_underlying(type) & 0xF); // (0–15) → 4 bits
    key |= (color ? 1 : 0)  << 4; // 1 bit
    key |= (flat_shaded ? 1 : 0) << 9; // 1 bit
    key |= (fog ? 1 : 0) << 10; // 1 bit
    key |= (lights.directional & 0xF) << 5; // (0–15) → 4 bits
    key |= (lights.point & 0xF) << 11; // (0–15) → 4 bits
    key |= (lights.spot & 0xF) << 15; // (0–15) → 4 bits
    key |= (albedo_map ? 1 : 0) << 19; // 1 bit
    key |= (alpha_map ? 1 : 0) << 20; // 1 bit
    key |= (normal_map ? 1 : 0) << 21; // 1 bit
    key |= (emissive_map ? 1 : 0) << 22; // 1 bit
    key |= (two_sided ? 1 : 0) << 23; // 1 bit
    key |= (instancing ? 1 : 0) << 24; // 1 bit
    key |= (vertex_color ? 1 : 0) << 25; // 1 bit
    key |= (tangent ? 1 : 0) << 26; // 1 bit
    key |= (specular_map ? 1 : 0) << 27; // 1 bit
    key |= (texture_map ? 1 : 0) << 28; // 1 bit
    key |= (size_attenuation ? 1 : 0) << 29; // 1 bit

    if (type == Material::Type::ShaderMaterial) {
        math::HashCombine(key, shader_material_id);
    }
}

}