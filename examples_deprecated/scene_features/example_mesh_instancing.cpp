/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "example_mesh_instancing.hpp"

#include <array>

using namespace vglx;

namespace {

auto colors = std::array<Color, 10> {
    0xE63946, // red
    0xF1A208, // orange
    0xF6E05E, // warm yellow
    0x90BE6D, // green
    0x43AA8B, // teal
    0x577590, // muted blue
    0x277DA1, // blue
    0x4D908E, // desaturated teal
    0xBC6C25, // earthy orange
    0x8E3B46  // wine red
};

}

ExampleMeshInstancing::ExampleMeshInstancing(Camera* camera) {
    show_context_menu_ = false;

    Add(OrbitControls::Create(camera, {.radius = 5.0f}));

    Add(AmbientLight::Create({.color = 0xFFFFFF, .intensity = .3f}));
    Add(PointLight::Create({.color = 0xFFFFFF, .intensity = 1.0f}))->TranslateZ(30.0f);

    const auto geometry = BoxGeometry::Create({1.0f, 1.0f, 1.0f});
    const auto material = PhongMaterial::Create(0xFFFFFF);
    boxes_ = Add(InstancedMesh::Create(geometry, material, 2500));

    for (auto i = 0; i < 50; ++i) {
        for (auto j = 0; j < 50; ++j) {
            auto t = Transform3 {};
            t.SetPosition({i * 2.0f - 49.0f, j * 2.0f - 49.0f, 0.0f});
            boxes_->SetTransformAt(j * 50 + i, t);
        }
    }

    for (auto i = 0; i < 2500; ++i) {
        boxes_->SetColorAt(i, colors[i % 10]);
    }
}

auto ExampleMeshInstancing::OnUpdate(float delta) -> void {
    auto t = Transform3 {};
    t.Rotate(Vector3::Right(), 1.0f * delta);
    for (auto i = 0; i < boxes_->Count(); ++i) {
        boxes_->SetTransformAt(i, boxes_->GetTransformAt(i) * t.Get());
    }
}