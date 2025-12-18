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

using namespace vglx;

namespace {

auto plane_geometry = PlaneGeometry::Create();
auto plane_material = PhongMaterial::Create();

}

ExampleSandbox::ExampleSandbox() {
    show_context_menu_ = false;

    Add(PointLight::Create({0xFFFFFF, 1.0f}))->transform.Translate({0.5f, 0.5f, 0.8f});
    Add(Mesh::Create(plane_geometry, plane_material))->SetScale(2.0f);
}

auto ExampleSandbox::OnAttached(SharedContextPointer context) -> void {
    context->texture_loader->LoadAsync(
        "assets/checker/checker.tex",
        [this](auto result) {
            if (result) {
                plane_material->albedo_map = result.value();
            }
        }
    );

    Add(OrbitControls::Create(
        context->camera, {
            .radius = 4.0f,
            .pitch = math::pi_over_6,
            .yaw = math::pi_over_6
        })
    );
}