/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "example_runner.hpp"

#include <print>

auto GetCamera() {
    auto camera = vglx::PerspectiveCamera::Create({
        .fov = vglx::math::DegToRad(60.0f),
        .aspect = static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight),
        .near = 0.3f,
        .far = 1000.0f
    });

    camera->TranslateZ(3.0f);

    return camera;
}

auto GetScene() {
    auto scene = vglx::Scene::Create();

    auto env_map = vglx::LoadCubeTexture({
        .positive_x = ASSETS_DIR "/skybox/positive_x.jpg",
        .negative_x = ASSETS_DIR "/skybox/negative_x.jpg",
        .positive_y = ASSETS_DIR "/skybox/positive_y.jpg",
        .negative_y = ASSETS_DIR "/skybox/negative_y.jpg",
        .positive_z = ASSETS_DIR "/skybox/positive_z.jpg",
        .negative_z = ASSETS_DIR "/skybox/negative_z.jpg",
    });

    if (env_map.has_value()) {
        scene->background = env_map.value();
    } else {
        std::print(stderr, "{}", env_map.error());
        return std::unique_ptr<vglx::Scene>(nullptr);
    }

    auto geometry = vglx::SphereGeometry::Create({
        .width_segments = 64,
        .height_segments = 32
    });

    auto material = vglx::PhongMaterial::Create({0xFFFFFF});
    material->environment_map = env_map.value();
    material->reflectivity = 0.5f;

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
    if (scene == nullptr) {
        return 1;
    }

    return RunExample(scene.get(), camera.get(), "Environment Map Phong");
}