/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "example_directional_light.hpp"

#include "ui_helpers.hpp"

#include <vglx/helpers.hpp>
#include <vglx/primitives.hpp>
#include <vglx/scene.hpp>

using namespace vglx;

ExampleDirectionalLight::ExampleDirectionalLight() {
    Add(Grid::Create({
        .color = 0x333333,
        .size = 4,
        .divisions = 16
    }));

    phong_material_ = PhongMaterial::Create(0xCCCCCC);
    const auto mesh = Mesh::Create(
        SphereGeometry::Create({
            .radius = 0.5f,
            .width_segments = 32,
            .height_segments = 32
        }),
        phong_material_
    );

    mesh->transform.Translate({0.0f, 0.5f, 0.0f});
    Add(mesh);

    Add(AmbientLight::Create({
        .color = 0xFFFFFF,
        .intensity = 0.15f
    }));

    directional_light_ = DirectionalLight::Create({
        .color = 0xFFFFFF,
        .intensity = 1.0f,
        .target = mesh
    });

    directional_light_->transform.Translate({2.0f, 2.0f, -2.0f});
    directional_light_->SetDebugMode(true);
    Add(directional_light_);
}

auto ExampleDirectionalLight::OnAttached(SharedContextPointer context) -> void {
    Add(OrbitControls::Create(context->camera, {
        .radius = 5.0f,
        .pitch = math::DegToRad(25.0f),
        .yaw = math::DegToRad(45.0f)
    }));
}

auto ExampleDirectionalLight::ContextMenu() -> void {
    auto _ = true;

    UIColor("color", &directional_light_->color[0], _, "light-color");
    UISliderFloat("intensity", directional_light_->intensity, 0.0f, 1.0f, _, 160.0f);

    UISeparator();

    UIText("Material");
    UIColor("color", &phong_material_->color[0], _, "material-color");
    UIColor("specular", &phong_material_->specular[0], _);
    UISliderFloat("shininess", phong_material_->shininess, 0.0f, 128.0f, _, 160.0f);
}