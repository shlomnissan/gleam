/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "example_model_loader.hpp"

#include "ui_helpers.hpp"

#include <vglx/core.hpp>
#include <vglx/helpers.hpp>
#include <vglx/lights.hpp>
#include <vglx/primitives.hpp>

#include <print>

using namespace vglx;

namespace {

auto handle = MeshLoadHandle {};

}

ExampleModelLoader::ExampleModelLoader() {
    show_context_menu_ = true;

    sphere_ = Add(Mesh::Create(
        SphereGeometry::Create({.radius = 5.0f}),
        PhongMaterial::Create(0x000011)
    ));
    sphere_->GetMaterial()->two_sided = true;

    sphere_->Add(AmbientLight::Create({
        .color = 0xFFFFFF,
        .intensity = 0.3f
    }));

    Add(PointLight::Create({
        .color = 0xFFFFFF,
        .intensity = 1.0f
    }))->transform.Translate({2.0f, 2.5f, 4.0f});

    Add(PointLight::Create({
        .color = 0xFAA916,
        .intensity = 1.0f
    }))->transform.Translate({-2.0f, 2.5f, -3.0f});
}

auto ExampleModelLoader::OnAttached(SharedContextPointer context) -> void {
    Add(OrbitControls::Create(context->camera, {
        .radius = 4.0f,
        .pitch = math::DegToRad(20.0f),
        .yaw = math::DegToRad(15.0f)
    }));

    handle = context->mesh_loader->LoadAsync(
        ASSETS_DIR "/lps_head/lps_head.obj"
    );
}

auto ExampleModelLoader::OnUpdate(float delta) -> void {
    if (auto mesh = handle.TryTake()) {
        model_ = sphere_->Add(std::move(mesh.value()));
        model_->RotateY(math::pi_over_2);

        auto ptr = static_cast<Mesh*>(model_->GetChildren().front().get());
        material_ = static_cast<PhongMaterial*>(ptr->GetMaterial().get());
        albedo_map_ = material_->albedo_map;
        normal_map_ = material_->normal_map;
        specular_map_ = material_->specular_map;
    }

    if (albedo_map_ != nullptr && !!material_->albedo_map != show_albedo_map_) {
        material_->albedo_map = show_albedo_map_ ? albedo_map_ : nullptr;
    }
    if (normal_map_ != nullptr && !!material_->normal_map != show_normal_map_) {
        material_->normal_map = show_normal_map_ ? normal_map_ : nullptr;
    }
    if (specular_map_ != nullptr && !!material_->specular_map != show_specular_map_) {
        material_->specular_map = show_specular_map_ ? specular_map_ : nullptr;
    }

    sphere_->RotateY(0.1f * delta);
}

auto ExampleModelLoader::ContextMenu() -> void {
    auto _ = false;
    UICheckbox("albedo_map", show_albedo_map_, _);
    UICheckbox("normal_map", show_normal_map_, _);
    UICheckbox("specular_map", show_specular_map_, _);
}