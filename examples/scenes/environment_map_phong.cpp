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

    camera->transform.Translate({0.0f, 0.0f, 6.0f});

    return camera;
}

auto GetScene() -> std::expected<std::unique_ptr<vglx::Scene>, std::string> {
    auto scene = vglx::Scene::Create();

    auto result = vglx::LoadCubeTexture({
        .positive_x = ASSETS_DIR "/skybox_mountains/positive_x.jpg",
        .negative_x = ASSETS_DIR "/skybox_mountains/negative_x.jpg",
        .positive_y = ASSETS_DIR "/skybox_mountains/positive_y.jpg",
        .negative_y = ASSETS_DIR "/skybox_mountains/negative_y.jpg",
        .positive_z = ASSETS_DIR "/skybox_mountains/positive_z.jpg",
        .negative_z = ASSETS_DIR "/skybox_mountains/negative_z.jpg",
    });

    if (!result.has_value()) {
        return std::unexpected(result.error());
    }

    auto env_map = *result;

    scene->background = env_map;

    auto geometry = vglx::SphereGeometry::Create({
        .radius = 0.8f,
        .width_segments = 64,
        .height_segments = 32
    });

    scene->Add(vglx::Mesh::Create(geometry, vglx::PhongMaterial::Create({
        .color = 0xFFFFFF,
        .reflectivity = 0.5f,
        .environment_map = env_map
    })))->transform.Translate({-2.3f, 0.0f, 0.0f});

    scene->Add(vglx::Mesh::Create(geometry, vglx::PhongMaterial::Create({
        .color = 0xFF0000,
        .reflectivity = 0.7f,
        .environment_map = env_map
    })));

    scene->Add(vglx::Mesh::Create(geometry, vglx::PhongMaterial::Create({
        .color = 0x0000FF,
        .reflectivity = 0.9f,
        .environment_map = env_map
    })))->transform.Translate({2.3f, 0.0f, 0.0f});

    scene->Add(vglx::AmbientLight::Create({
        .color = 0xFFFFFF,
        .intensity = 0.3f
    }));

    scene->Add(vglx::DirectionalLight::Create({
        .color = 0xFFFFFF,
        .intensity = 0.7f,
    }))->transform.Translate({0.0f, 200.0f, 0.0f});

    scene->Add(vglx::DirectionalLight::Create({
        .color = 0xFFFFFF,
        .intensity = 0.7f,
    }))->transform.Translate({-100.0f, -200.0f, -100.0f});

    scene->Add(vglx::DirectionalLight::Create({
        .color = 0xFFFFFF,
        .intensity = 1.0f,
    }))->transform.Translate({100.0f, 200.0f, 100.0f});

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
        .window_title = "Environment Map Phong"
    });
}
