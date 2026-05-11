/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "example_fog.hpp"

#include "ui_helpers.hpp"

#include <vglx/core.hpp>
#include <vglx/helpers.hpp>
#include <vglx/lights.hpp>
#include <vglx/materials.hpp>
#include <vglx/primitives.hpp>
#include <vglx/scene.hpp>

using namespace vglx;

ExampleFog::ExampleFog(Camera* camera) {
    Add(OrbitControls::Create(camera, {
        .radius = 3.0f,
        .pitch = math::DegToRad(25.0f),
        .yaw = math::DegToRad(-25.0f)
    }));

    auto geometry = BoxGeometry::Create();
    auto material = PhongMaterial::Create(0x049EF4);

    for (auto i = 0; i < 10; ++i) {
        Add(
            Mesh::Create(geometry, material)
        )->TranslateZ(-1.5f * static_cast<float>(i));
    }

    Add(AmbientLight::Create({
        .color = 0xFFFFFF,
        .intensity = 0.3f
    }));

    Add(PointLight::Create({
        .color = 0xFFFFFF,
        .intensity = 1.0f,
    }))->transform.Translate({2.0f, 2.0f, 2.0f});

    fog = Fog::CreateLinear(0x444444, 2.0f, 6.0f);
}

auto ExampleFog::ContextMenu() -> void {
    auto _ = true;
    static auto curr_fog_function = std::string {"linear"};
    static auto fog_function = std::array<const char*, 2> {
        "linear", "exponential"
    };

    UIColor("color", &fog->color[0], _);
    UIDropDown("function", fog_function, curr_fog_function,
      [this](std::string_view str) {
        curr_fog_function = str;
        if (str == "linear") fog = Fog::CreateLinear(fog->color, 2.0f, 6.0f);
        if (str == "exponential") fog = Fog::CreateExponential(fog->color, 0.2f);
    });

    if (fog->GetType() == Fog::Type::Linear) {
        UISliderFloat("near", fog->near, 0.0f, 20.0f, _, 160.0f);
        UISliderFloat("far", fog->far, 0.0f, 20.0f, _, 160.0f);
    }

    if (fog->GetType() == Fog::Type::Exponential) {
        UISliderFloat("density", fog->density, 0.0f, 1.0f, _, 160.0f);
    }
}