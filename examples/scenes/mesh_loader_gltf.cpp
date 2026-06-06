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

    auto result = vglx::LoadMesh(ASSETS_DIR "/barbarian/scene.gltf");
    if (!result.has_value()) {
        return std::unexpected(result.error());
    }

    result.value()->transform.Translate({0.0f, -1.1f, 0.0f});

    scene->Add(std::move(result.value()));

    scene->Add(vglx::AmbientLight::Create({
        .color = 0xFFFFFF,
        .intensity = 1.0
    }));

    scene->Add(vglx::PointLight::Create({
        .color = 0xFFFFFF,
        .intensity = 1.5f
    }))->transform.Translate({2.0f, 6.0f, 10.0f});

    scene->Add(vglx::PointLight::Create({
        .color = 0xFAA916,
        .intensity = 1.5f
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
        .window_title = "Mesh Loader glTF",
        .clear_color = vglx::Color {0xC9C9C9}
    });
}
