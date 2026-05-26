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

    camera->transform.SetPosition({0.0f, 1.0f, 5.0f});

    return camera;
}

auto GetScene() -> std::expected<std::unique_ptr<vglx::Scene>, std::string> {
    auto scene = vglx::Scene::Create();

    auto hdri_background = vglx::LoadHDRTexture(ASSETS_DIR "/hdri/rogland_clear_night.hdr");
    if (!hdri_background.has_value()) {
        return std::unexpected(hdri_background.error());
    }

    scene->background = *hdri_background;

    auto sphere = vglx::SphereGeometry::Create({
        .width_segments = 64,
        .height_segments = 32
    });

    auto sphere_material = vglx::PBRMaterial::Create(0xB3B3B3);
    sphere_material->metallic = 1.0f;
    sphere_material->roughness = 0.2f;

    scene->Add(vglx::Mesh::Create(sphere, sphere_material));

    auto plane = vglx::PlaneGeometry::Create({
        .width = 20.0f,
        .height = 20.0f
    });

    auto plane_material = vglx::PBRMaterial::Create(0x808080);
    plane_material->metallic = 0.0f;
    plane_material->roughness = 0.8f;

    scene->Add(
        vglx::Mesh::Create(plane, plane_material)
    )->transform.Rotate(vglx::Vector3::Right(), vglx::math::DegToRad(-90.0f));

    scene->Add(vglx::AmbientLight::Create({
        .color = 0xFFFFFF,
        .intensity = .05f
    }));

    scene->Add(vglx::PointLight::Create({
        .color = 0xFFFFFF,
        .intensity = 50.0f,
        .attenuation = {
            .base = 1.0f,
            .linear = 0.0f,
            .quadratic = 1.0f
        }
    }))->transform.Translate({1.5f, 3.0f, 1.5f});

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
        .window_title = "Tone Mapping",
        .clear_color = 0x000000,
        .tone_mapping = vglx::Renderer::ToneMapping::ACESFilmic
    });
}
