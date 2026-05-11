/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "example_sandbox.hpp"

#include <print>

#include <vglx/helpers.hpp>
#include <vglx/lights.hpp>
#include <vglx/loaders.hpp>
#include <vglx/materials.hpp>
#include <vglx/math.hpp>
#include <vglx/primitives.hpp>
#include <vglx/textures.hpp>

using namespace vglx;

ExampleSandbox::ExampleSandbox(Camera* camera) {
    show_context_menu_ = false;

    Add(OrbitControls::Create(camera, {
        .radius = 4.5f,
        .yaw = 0.5f
    }));

    Add(AmbientLight::Create({.color = 0xFFFFFF, .intensity = 0.2f}));

    Add(PointLight::Create({
        .color = 0xFFFFFF,
        .intensity = 0.6f
    }))->transform.Translate({2.0f, 2.5f, 0.0f});

    auto mesh = LoadMesh(ASSETS_DIR "/robot/robot.obj");
    if (mesh.has_value()) {
        mesh_ = Add(std::move(mesh.value()));
        mesh_->SetScale(0.03f);
        mesh_->RotateY(math::DegToRad(180.0f));
        mesh_->TranslateY(-1.0f);
    } else {
        std::println(stderr, "{}", mesh.error());
    }

    auto texture = LoadCubeTexture({
        .positive_x = ASSETS_DIR "/skybox/positive_x.jpg",
        .negative_x = ASSETS_DIR "/skybox/negative_x.jpg",
        .positive_y = ASSETS_DIR "/skybox/positive_y.jpg",
        .negative_y = ASSETS_DIR "/skybox/negative_y.jpg",
        .positive_z = ASSETS_DIR "/skybox/positive_z.jpg",
        .negative_z = ASSETS_DIR "/skybox/negative_z.jpg",
    });
    if (texture.has_value()) {
        background = texture.value();
    } else {
        std::println(stderr, "{}", texture.error());
    }
}

