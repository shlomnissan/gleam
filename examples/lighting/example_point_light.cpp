/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "example_point_light.hpp"

#include "ui_helpers.hpp"

using namespace vglx;

ExamplePointLight::ExamplePointLight(Camera* camera) {
    Add(OrbitControls::Create(camera, {
        .radius = 5.0f,
        .pitch = math::DegToRad(25.0f),
        .yaw = math::DegToRad(45.0f)
    }));

    Add(Grid::Create({
        .color = 0x333333,
        .size = 4,
        .divisions = 16
    }));

    phong_material_ = PhongMaterial::Create(0xCCCCCC);
    auto mesh = Add(Mesh::Create(
        SphereGeometry::Create({
            .radius = 0.5f,
            .width_segments = 32,
            .height_segments = 32
        }),
        phong_material_
    ));

    mesh->transform.Translate({0.0f, 0.5f, 0.0f});

    Add(AmbientLight::Create({
        .color = 0xFFFFFF,
        .intensity = .15f
    }));

    point_light_ = Add(PointLight::Create({
        .color = 0xFFFFFF,
        .intensity = 1.0f,
    }));

    point_light_->transform.Translate({1.0f, 2.0f, -2.0f});
    point_light_->SetDebugMode(true);
}

auto ExamplePointLight::ContextMenu() -> void {
    auto _ = true;

    UIColor("color", &point_light_->color[0], _, "light-color");
    UISliderFloat("intensity", point_light_->intensity, 0.0f, 1.0f, _, 160.0f);
    UIText("Attenuation");
    UISliderFloat("base", point_light_->attenuation.base, 0.0f, 2.0f, _, 160.0f);
    UISliderFloat("linear", point_light_->attenuation.linear, 0.0f, 1.0f, _, 160.0f);
    UISliderFloat("quadratic", point_light_->attenuation.quadratic, 0.0f, 2.0f, _, 160.0f);

    UISeparator();

    UIText("Material");
    UIColor("color", &phong_material_->color[0], _, "material-color");
    UIColor("specular", &phong_material_->specular_color[0], _);
    UISliderFloat("shininess", phong_material_->shininess, 0.0f, 128.0f, _, 160.0f);
}