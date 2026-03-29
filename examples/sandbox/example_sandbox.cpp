/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "example_sandbox.hpp"
#include "vglx/loaders/load_handle.hpp"

#include <vglx/helpers.hpp>
#include <vglx/lights.hpp>
#include <vglx/materials.hpp>
#include <vglx/math.hpp>
#include <vglx/primitives.hpp>
#include <vglx/textures.hpp>

using namespace vglx;

namespace {

auto mesh_handle = MeshLoadHandle {};
auto skybox_handle = CubeTextureLoadHandle {};

}

ExampleSandbox::ExampleSandbox() {
    show_context_menu_ = false;

    Add(AmbientLight::Create({.color = 0xFFFFFF, .intensity = 0.2f}));

    Add(PointLight::Create({
        .color = 0xFFFFFF,
        .intensity = 0.6f
    }))->transform.Translate({2.0f, 2.5f, 0.0f});
}

auto ExampleSandbox::OnAttached(SharedContextPointer context) -> void {
    mesh_handle = context->mesh_loader->LoadAsync(ASSETS_DIR "/robot/robot.obj");
    skybox_handle = context->cube_texture_loader->LoadAsync({
        .positive_x = ASSETS_DIR "/skybox/positive_x.jpg",
        .negative_x = ASSETS_DIR "/skybox/negative_x.jpg",
        .positive_y = ASSETS_DIR "/skybox/positive_y.jpg",
        .negative_y = ASSETS_DIR "/skybox/negative_y.jpg",
        .positive_z = ASSETS_DIR "/skybox/positive_z.jpg",
        .negative_z = ASSETS_DIR "/skybox/negative_z.jpg",
    });

    Add(OrbitControls::Create(context->camera, {
        .radius = 4.5f,
        .yaw = 0.5f
    }));
}

auto ExampleSandbox::OnUpdate(float dt) -> void {
    if (auto result = mesh_handle.TryTake()) {
        mesh_ = Add(std::move(result.value()));
        mesh_->SetScale(0.03f);
        mesh_->RotateY(math::DegToRad(180.0f));
        mesh_->TranslateY(-1.0f);
    }

    if (auto result = skybox_handle.TryTake()) {
        background = result.value();
    }
}