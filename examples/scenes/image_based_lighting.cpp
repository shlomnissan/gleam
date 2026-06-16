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

    auto result = vglx::LoadHDRTexture(ASSETS_DIR "/hdri/ferndale_studio_04_4k.hdr");
    if (!result.has_value()) {
        return std::unexpected(result.error());
    }

    scene->background = *result;
    scene->environment = *result;

    return scene;
}

auto main() -> int {
    auto camera = get_camera();
    auto scene = get_scene();
    if (!scene.has_value()) {
        std::print(stderr, "{}", scene.error());
        return 1;
    }

    scene->get()->Add(vglx::OrbitControls::Create(camera.get(), {.radius = 6.0f}));

    return run_example(scene->get(), camera.get(), {
        .window_title = "Image-Based Lighting",
        .tone_mapping = vglx::Renderer::ToneMapping::ACESFilmic
    });
}
