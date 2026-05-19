/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "example_blending.hpp"

#include "ui_helpers.hpp"

using namespace vglx;

ExampleBlending::ExampleBlending(Camera* camera) {
    Add(OrbitControls::Create(camera, {.radius = 3.0f}));

    auto opaque_material = PhongMaterial::Create();
    opaque_material->color = 0xE5BEED;
    opaque_material->two_sided = true;

    auto plane_geometry = PlaneGeometry::Create({.width = 2.0f, .height = 2.0f});
    Add(Mesh::Create(plane_geometry, opaque_material))->TranslateZ(-0.5f);

    transparent_material_ = PhongMaterial::Create();
    transparent_material_->color = 0x049EF4;
    transparent_material_->transparent = true;
    transparent_material_->opacity = 0.9f;

    auto sphere_geometry = SphereGeometry::Create({.radius = 0.5f});
    Add(Mesh::Create(sphere_geometry, transparent_material_))->TranslateX(0.7f);

    Add(AmbientLight::Create({
        .color = 0xFFFFFF,
        .intensity = .3f
    }));

    Add(PointLight::Create({
        .color = 0xFFFFFF,
        .intensity = 1.0f
    }))->transform.Translate({2.0f, 2.0f, 2.0f});
}

auto ExampleBlending::ContextMenu() -> void {
    auto _ = true;
    static auto curr_blend_mode = std::string {"normal"};
    static auto blending_modes = std::array<const char*, 5> {
        "none", "normal", "additive", "subtractive", "multiply"
    };

    UIDropDown("mode", blending_modes, curr_blend_mode,
      [this](std::string_view str) {
        using enum Material::Blending;
        curr_blend_mode = str;
        if (str == "none") transparent_material_->blending = None;
        if (str == "normal") transparent_material_->blending = Normal;
        if (str == "additive") transparent_material_->blending = Additive;
        if (str == "subtractive") transparent_material_->blending = Subtractive;
        if (str == "multiply") transparent_material_->blending = Multiply;
    });

    UISliderFloat("opacity", transparent_material_->opacity, 0.0f, 1.0f, _, 160.0f);
}