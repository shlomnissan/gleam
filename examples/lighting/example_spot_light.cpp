/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "example_spot_light.hpp"

#include "ui_helpers.hpp"

#include <vglx/helpers.hpp>
#include <vglx/primitives.hpp>
#include <vglx/scene.hpp>

using namespace vglx;

ExampleSpotLight::ExampleSpotLight() {
    Add(Grid::Create({
        .color = 0x333333,
        .size = 4,
        .divisions = 16
    }));

    phong_material_ = PhongMaterial::Create(0xCCCCCC);
    phong_material_->polygon_offset_factor = -1.0f;
    phong_material_->polygon_offset_units = 1.0f;

    auto mesh = Mesh::Create(
        PlaneGeometry::Create({.width = 3, .height = 3}),
        phong_material_
    );

    mesh->transform.Rotate(Vector3::Right(), math::DegToRad(-90.0f));
    Add(mesh);

    Add(AmbientLight::Create({
        .color = 0xFFFFFF,
        .intensity = 0.15f
    }));

    spot_light_ = SpotLight::Create({
        .color = 0xFFFFFF,
        .intensity = 1.0f,
        .angle = math::DegToRad(10.0f),
        .penumbra = 0.3f,
        .target = nullptr,
        .attenuation = {
            .base = 1.0f,
            .linear = 0.0f,
            .quadratic = 0.0f
        }
    });

    spot_light_->transform.Translate({2.0f, 2.0f, -1.0f});
    spot_light_->SetDebugMode(true);
    Add(spot_light_);
}

auto ExampleSpotLight::OnAttached(SharedContextPointer context) -> void {
    Add(OrbitControls::Create(context->camera, {
        .radius = 5.0f,
        .pitch = math::DegToRad(25.0f),
        .yaw = math::DegToRad(45.0f)
    }));
}

auto ExampleSpotLight::ContextMenu() -> void {
    auto _ = true;

    UIColor("color", &spot_light_->color[0], _, "light-color");
    UISliderFloat("intensity", spot_light_->intensity, 0.0f, 1.0f, _, 160.0f);
    UISliderFloat("angle", spot_light_->angle, 0.1f, math::pi_over_2, _, 160.0f);
    UISliderFloat("penumbra", spot_light_->penumbra, 0.0f, 1.0f, _, 160.0f);
    UIText("Attenuation");
    UISliderFloat("base", spot_light_->attenuation.base, 0.0f, 2.0f, _, 160.0f);
    UISliderFloat("linear", spot_light_->attenuation.linear, 0.0f, 1.0f, _, 160.0f);
    UISliderFloat("quadratic", spot_light_->attenuation.quadratic, 0.0f, 2.0f, _, 160.0f);

    UISeparator();

    UIText("Material");
    UIColor("color", &phong_material_->color[0], _, "material-color");
    UIColor("specular", &phong_material_->specular[0], _);
    UISliderFloat("shininess", phong_material_->shininess, 0.0f, 128.0f, _, 160.0f);
}