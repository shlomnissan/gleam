/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/materials/shader_material.hpp"
#include "vglx/math/utilities.hpp"

namespace vglx {

ShaderMaterial::ShaderMaterial(Parameters params)
  : vertex_shader_(std::move(params.vertex_shader)),
    fragment_shader_(std::move(params.fragment_shader)),
    textures_(std::move(params.textures))
{
    uniforms_.reserve(params.uniforms.size());

    math::HashCombine(shader_material_id_, vertex_shader_);
    math::HashCombine(shader_material_id_, fragment_shader_);

    for (const auto& [k, v] : params.uniforms) uniforms_.try_emplace(k, v);
}

auto ShaderMaterial::SetUniform(std::string_view name, UniformValue value) -> void {
    if (auto it = uniforms_.find(name); it != uniforms_.end()) {
        it->second = std::move(value);
    } else {
        uniforms_.try_emplace(std::string(name), std::move(value));
    }
}

auto ShaderMaterial::SetTexture(std::string_view name, std::shared_ptr<Texture> texture) -> void {
    for (auto& t : textures_) {
        if (t.name == name) {
            t.texture = std::move(texture);
            return;
        }
    }
    textures_.emplace_back(std::string(name), std::move(texture));
}

}