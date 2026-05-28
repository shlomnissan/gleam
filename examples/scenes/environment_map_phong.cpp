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

    camera->transform.Translate({0.0f, 0.0f, 5.0f});

    return camera;
}

auto GetScene() -> std::expected<std::unique_ptr<vglx::Scene>, std::string> {
    auto scene = vglx::Scene::Create();

    auto env_map = vglx::LoadCubeTexture({
        .positive_x = ASSETS_DIR "/skybox/positive_x.jpg",
        .negative_x = ASSETS_DIR "/skybox/negative_x.jpg",
        .positive_y = ASSETS_DIR "/skybox/positive_y.jpg",
        .negative_y = ASSETS_DIR "/skybox/negative_y.jpg",
        .positive_z = ASSETS_DIR "/skybox/positive_z.jpg",
        .negative_z = ASSETS_DIR "/skybox/negative_z.jpg",
    });

    if (!env_map.has_value()) {
        return std::unexpected(env_map.error());
    }

    scene->background = *env_map;

    auto geometry = vglx::SphereGeometry::Create({
        .width_segments = 64,
        .height_segments = 32
    });

    auto material = vglx::PhongMaterial::Create({
        .color = 0xFFFFFF,
        .reflectivity = 0.7f,
        .environment_map = *env_map
    });

    scene->Add(vglx::Mesh::Create(geometry, material));

    scene->Add(vglx::AmbientLight::Create({
        .color = 0xFFFFFF,
        .intensity = .3f
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
