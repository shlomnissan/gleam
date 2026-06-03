/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <expected>
#include <memory>
#include <print>
#include <string>

#include "example_runner.hpp"

auto GetCamera() {
    auto camera = vglx::PerspectiveCamera::Create({
        .fov = vglx::math::DegToRad(60.0f),
        .aspect = static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight),
        .near = 0.3f,
        .far = 1000.0f
    });

    camera->transform.Translate({0.0f, 0.0f, 3.5f});

    return camera;
}

auto GetScene() -> std::expected<std::unique_ptr<vglx::Scene>, std::string> {
    auto scene = vglx::Scene::Create();
    auto sphere = scene->Add(vglx::Mesh::Create(
        vglx::SphereGeometry::Create({.radius = 5.0f}),
        vglx::PhongMaterial::Create({.color = 0x000011})
    ));

    sphere->GetMaterial()->two_sided = true;

    auto result = vglx::LoadMesh(ASSETS_DIR "/lps_head/lps_head.obj");
    if (!result.has_value()) {
        return std::unexpected(result.error());
    }

    auto mesh = sphere->Add(std::move(result.value()));
    mesh->transform.Rotate(vglx::Vector3::Right(), vglx::math::DegToRad(15.0f));
    mesh->transform.Rotate(vglx::Vector3::Up(), vglx::math::DegToRad(90.0f));

    sphere->Add(vglx::AmbientLight::Create({
        .color = 0xFFFFFF,
        .intensity = 0.3f
    }));

    sphere->Add(vglx::PointLight::Create({
        .color = 0xFFFFFF,
        .intensity = 0.9f
    }))->transform.Translate({2.0f, 6.0f, 10.0f});

    sphere->Add(vglx::PointLight::Create({
        .color = 0xFAA916,
        .intensity = 1.0f
    }))->transform.Translate({-2.0f, 6.0f, -4.0f});

    return scene;
}

auto main() -> int {
    auto camera = GetCamera();
    auto scene = GetScene();
    if (!scene.has_value()) {
        std::print(stderr, "{}", scene.error());
        return 1;
    }

    return RunExample(scene->get(), camera.get(), {
        .window_title = "Mesh Loader OBJ",
        .clear_color = vglx::Color {0x6C6C6C}
    });
}
