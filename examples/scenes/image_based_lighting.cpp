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

    auto result_background = vglx::LoadHDRTexture(ASSETS_DIR "/hdri/qwantani_dusk_2_puresky_4k.hdr");
    if (!result_background.has_value()) {
        return std::unexpected(result_background.error());
    }

    scene->background = *result_background;
    scene->environment = *result_background;

    auto result_model = vglx::LoadMesh(ASSETS_DIR "/damaged_helmet/damaged_helmet.gltf");
    if (!result_model.has_value()) {
        return std::unexpected(result_model.error());
    }

    scene->Add(std::move(result_model.value()));

    scene->Add(vglx::AmbientLight::Create({
        .color = 0xFFFFFF,
        .intensity = 1.0
    }));

    scene->Add(vglx::PointLight::Create({
        .color = 0xFFFFFF,
        .intensity = 128.0f
    }))->transform.Translate({2.0f, 6.0f, 10.0f});

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
        .radius = 3.0f,
        .yaw = 0.5f,
    }));

    return run_example(scene->get(), camera.get(), {
        .window_title = "Image-Based Lighting",
        .tone_mapping = vglx::Renderer::ToneMapping::ACESFilmic
    });
}
