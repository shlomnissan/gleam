/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "example_pbr_material.hpp"

#include "ui_helpers.hpp"

#include <print>

#include <print>

using namespace vglx;

ExamplePBRMaterial::ExamplePBRMaterial(Camera* camera) {
    Add(OrbitControls::Create(camera, {.radius = 5.0f}));

    auto geometry = TorusKnotGeometry::Create({
        .radius = 1.0f,
        .tube = 0.3f,
        .tubular_segments = 200,
        .radial_segments = 32
    });

    material_ = PBRMaterial::Create(0x049EF4);
    mesh_ = Add(Mesh::Create(geometry, material_));

    Add(AmbientLight::Create({
        .color = 0xFFFFFF,
        .intensity = 1.0f
    }));

    Add(DirectionalLight::Create({
        .color = 0xFFFFFF,
        .intensity = 3.0f,
    }))->transform.Translate({0.0f, 200.0f, 0.0f});

    Add(DirectionalLight::Create({
        .color = 0xFFFFFF,
        .intensity = 3.0f,
    }))->transform.Translate({100.0f, 200.0f, 100.0f});

    Add(DirectionalLight::Create({
        .color = 0xFFFFFF,
        .intensity = 3.0f,
    }))->transform.Translate({-100.0f, -200.0f, -100.0f});

    auto texture = LoadTexture(ASSETS_DIR "/checker/checker.png");
    if (texture.has_value()) {
        texture_ = texture.value();
    } else {
        std::println(stderr, "{}", texture.error());
    }
}

auto ExamplePBRMaterial::OnUpdate(float delta) -> void {
    mesh_->transform.Rotate(Vector3::Up(), 1.0f * delta);
    mesh_->transform.Rotate(Vector3::Right(), 1.0f * delta);
}

auto ExamplePBRMaterial::ContextMenu() -> void {
    auto _ = false;
    static auto curr_texture = std::string {"none"};
    static auto textures = std::array<const char*, 2> {
        "none", "checkerboard"
    };

    UIColor("color", &material_->color[0], _);
    UISliderFloat("metallic", material_->metallic, 0.0f, 1.0f, _, 160.0f);
    UISliderFloat("roughness", material_->roughness, 0.0f, 1.0f, _, 160.0f);
    UISliderFloat("ao_intensity", material_->ao_intensity, 0.0f, 1.0f, _, 160.0f);
    UIDropDown("texture", textures, curr_texture,
      [this](std::string_view str) {
        curr_texture = str;
        if (str == "none") material_->albedo_map = nullptr;
        if (str == "checkerboard") material_->albedo_map = texture_;
    });

    UISeparator();

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
