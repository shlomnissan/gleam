/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <memory>

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

auto get_scene() {
    return ExampleScene::Create();
}

auto main() -> int {
    auto camera = get_camera();
    auto scene = get_scene();

    return run_example(scene.get(), camera.get(), {
        .window_title = "Terrain Fog",
        .clear_color = 0xEFD1B5u
    });
}
