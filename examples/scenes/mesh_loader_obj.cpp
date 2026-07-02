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

auto get_camera() {
    auto camera = vglx::PerspectiveCamera::Create({
        .fov = vglx::math::DegToRad(60.0f),
        .aspect = static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight),
        .near = 0.3f,
        .far = 1000.0f
    });

    return camera;
}

auto get_scene() -> std::expected<std::unique_ptr<vglx::Scene>, std::string> {
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
    mesh->transform.Rotate(vglx::Vector3::Up(), vglx::math::DegToRad(90.0f));

    sphere->Add(vglx::AmbientLight::Create({
        .color = 0xFFFFFF,
        .intensity = 0.3f
    }));

    sphere->Add(vglx::PointLight::Create({
        .color = 0xFFFFFF,
        .intensity = 128.0f
    }))->transform.Translate({2.0f, 6.0f, 10.0f});

    sphere->Add(vglx::PointLight::Create({
        .color = 0xFAA916,
        .intensity = 64.0f
    }))->transform.Translate({-2.0f, 6.0f, -4.0f});

    return scene;
}

auto main() -> int {
    auto camera = get_camera();
    auto scene = get_scene();
    if (!scene.has_value()) {
        std::print(stderr, "{}", scene.error());
        return 1;
    }

    scene->get()->Add(vglx::OrbitControls::Create(camera.get(), {
        .radius = 3.5f,
        .pitch = 0.25f
    }));

    return run_example(scene->get(), camera.get(), {
        .window_title = "Mesh Loader OBJ",
    });
}
