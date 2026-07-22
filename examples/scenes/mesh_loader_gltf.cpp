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

auto get_scene() -> std::expected<std::unique_ptr<ExampleScene>, std::string> {
    auto scene = ExampleScene::Create();

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
        .intensity = 128.0f
    }))->transform.Translate({2.0f, 6.0f, 10.0f});

    scene->Add(vglx::PointLight::Create({
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

    scene->get()->Add(vglx::OrbitControls::Create(camera.get(), {.radius = 3.5f}));

    return run_example(scene->get(), camera.get(), {
        .window_title = "Mesh Loader glTF",
        .clear_color = vglx::Color {0xC9C9C9}
    });
}
