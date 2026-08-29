/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <cassert>
#include <memory>
#include <random>
#include <utility>
#include <vector>

#include "example_runner.hpp"

constexpr auto world_width = 256;
constexpr auto world_height = 256;

auto rng {std::mt19937 {0u}};
auto dist {std::uniform_real_distribution<float> {0.0f, 100.0f}};

auto generate_heightfield() {
    auto size = world_width * world_height;
    auto data = std::vector<float>(size);

    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = dist(rng);
    }

    return data;
}

auto process_geometry(vglx::Geometry& geometry) {
    auto position_attribute = geometry.GetAttribute(vglx::BufferAttribute::kPosition);
    auto positions = position_attribute->GetData();
    auto heightfield = generate_heightfield();
    assert(positions.size() == heightfield.size() * 3);
    for (auto i = size_t {0}; i < heightfield.size(); ++i) {
        positions[i * 3 + 1] = heightfield[i];
    }

    position_attribute->SetData(std::move(positions));
    geometry.GenerateNormals();
}

auto get_camera() {
    auto camera = vglx::PerspectiveCamera::Create({
        .fov = vglx::math::DegToRad(60.0f),
        .aspect = static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight),
        .near = 0.3f,
        .far = 10'000.0f
    });

    return camera;
}

auto get_scene() {
    auto scene = ExampleScene::Create();

    scene->Add(vglx::DirectionalLight::Create({
        .color = 0xFFFFFFu,
        .intensity = 1.0f
    }))->transform.Translate({1500.0f, 1200.0f, 800.0f});

    scene->Add(vglx::AmbientLight::Create({
        .color = 0xFFFFFFu,
        .intensity = 0.3f
    }));

    auto mesh = scene->Add(vglx::Mesh::Create(
        vglx::PlaneGeometry::Create({
            .width = 7500,
            .height = 7500,
            .width_segments = world_width - 1,
            .height_segments = world_height - 1
        }),
        vglx::PhongMaterial::Create({.color = 0xFF0000u})
    ));

    process_geometry(*mesh->GetGeometry().get());

    return scene;
}

auto main() -> int {
    auto camera = get_camera();
    auto scene = get_scene();

    scene->Add(vglx::OrbitControls::Create(camera.get(), {.radius = 1000.0f, .pitch = 0.5f}));

    return run_example(scene.get(), camera.get(), {
        .window_title = "Terrain Fog",
        .clear_color = 0xEFD1B5u
    });
}
