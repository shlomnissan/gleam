/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "example_sandbox.hpp"

#include <vglx/helpers.hpp>
#include <vglx/lights.hpp>
#include <vglx/materials.hpp>
#include <vglx/math.hpp>
#include <vglx/primitives.hpp>
#include <vglx/textures.hpp>

using namespace vglx;

namespace {

auto loader_handle = MeshLoadHandle {};

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
    loader_handle = context->mesh_loader->LoadAsync("assets/robot/robot.msh");

    Add(OrbitControls::Create(
        context->camera, {
            .radius = 4.5f,
            .yaw = 0.5f
        })
    );
}

auto ExampleSandbox::OnUpdate(float dt) -> void {
    if (auto result = loader_handle.TryTake()) {
        mesh_ = Add(std::move(result.value()));
        mesh_->SetScale(0.03f);
        mesh_->RotateY(math::DegToRad(180.0f));
        mesh_->TranslateY(-1.0f);
    }
}