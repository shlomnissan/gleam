/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/materials/shader_material.hpp"

namespace vglx {

ShaderMaterial::ShaderMaterial(const Parameters& params)
  : vertex_shader_(params.vertex_shader),
    fragment_shader_(params.fragment_shader)
{
    uniforms_.reserve(params.uniforms.size());
    for (const auto& [k, v] : params.uniforms) uniforms_.emplace(k, v);
}

auto ShaderMaterial::SetUniform(std::string_view name, UniformValue value) -> void {
    if (auto it = uniforms_.find(name); it != uniforms_.end()) {
        it->second = std::move(value);
    } else {
        uniforms_.emplace(std::string(name), std::move(value));
    }
}

}