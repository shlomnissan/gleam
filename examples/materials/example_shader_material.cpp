/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "example_shader_material.hpp"

#include "ui_helpers.hpp"

#include <vglx/helpers.hpp>
#include <vglx/primitives.hpp>

using namespace vglx;

namespace {

constexpr auto vert_str = R"(
#version 410 core
#pragma inject_attributes

#include "snippets/vert_global_params.glsl"

void main() {
    #include "snippets/vert_main_varyings.glsl"
    gl_Position = u_Projection * v_Position;
})";

constexpr auto frag_str = R"(
#version 410 core
#pragma inject_attributes

precision mediump float;

#include "snippets/frag_global_params.glsl"
#include "snippets/frag_global_fog.glsl"

uniform float u_Time;
uniform vec2 u_Resolution;

void main() {
    vec2 uv = gl_FragCoord.xy / u_Resolution.xy;
    vec3 output_color = 0.5 + 0.5 * cos(u_Time + uv.xyx + vec3(0,2,4));

    #ifdef USE_FOG
        applyFog(output_color, v_ViewDepth);
    #endif

    v_FragColor = vec4(output_color, u_Opacity);
})";

}

ExampleShaderMaterial::ExampleShaderMaterial() {
    auto geometry = BoxGeometry::Create();

    material_ = ShaderMaterial::Create({
        .vertex_shader = vert_str,
        .fragment_shader = frag_str,
        .uniforms = {{"u_Time", 0.0f}}
    });

    fog = Fog::CreateExponential(0x444444, 0.3f);
    material_->fog = false;

    mesh_ = Add(Mesh::Create(geometry, material_));
}

auto ExampleShaderMaterial::OnAttached(SharedContextPointer context) -> void {
    Add(OrbitControls::Create(context->camera, {.radius = 3.0f}));
}

auto ExampleShaderMaterial::OnUpdate(float delta) -> void {
    mesh_->transform.Rotate(Vector3::Up(), 1.0f * delta);
    mesh_->transform.Rotate(Vector3::Right(), 1.0f * delta);
    material_->uniforms["u_Time"] = static_cast<float>(timer_.GetElapsedSeconds());
}

auto ExampleShaderMaterial::ContextMenu() -> void {
    auto _ = false;

    UICheckbox("transparent", material_->transparent, _);
    UISliderFloat("opacity", material_->opacity, 0.0f, 1.0f, _, 160.0f);

    UISeparator();

    UICheckbox("depth_test", material_->depth_test, _);
    UICheckbox("flat_shaded", material_->flat_shaded, _);
    UICheckbox("fog", material_->fog, _);
    UICheckbox("two_sided", material_->two_sided, _);
    UICheckbox("visible", material_->visible, _);
    UICheckbox("wireframe", material_->wireframe, _);
}