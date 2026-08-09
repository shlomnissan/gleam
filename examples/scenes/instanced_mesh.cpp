/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <memory>
#include <print>

#include "example_runner.hpp"

struct Scene : public ExampleScene {
    vglx::InstancedMesh* mesh {nullptr};

    Scene() {
        auto path = ASSETS_DIR "/textures/instance.jpg";
        auto texture = vglx::LoadTexture(path);
        if (!texture.has_value()) {
            std::println(stderr, "Failed to load texture {}", path);
        }

        mesh = this->Add(vglx::InstancedMesh::Create(
            vglx::BoxGeometry::Create(),
            vglx::UnlitMaterial::Create({
                .color = 0x049EF4,
                .texture_map = texture.has_value() ? texture.value() : nullptr
            }),
            /* count = */ 1
        ));

        mesh->SetTransformAt(0, vglx::Matrix4::Identity());
    }


    auto OnUpdate(float dt) -> void override {}
};

auto get_camera() {
    auto camera = vglx::PerspectiveCamera::Create({
        .fov = vglx::math::DegToRad(60.0f),
        .aspect = static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight),
        .near = 0.3f,
        .far = 1000.0f
    });

    return camera;
}

auto main() -> int {
    auto camera = get_camera();
    auto scene = std::make_unique<Scene>();

    scene.get()->Add(vglx::OrbitControls::Create(camera.get(), {
        .radius = 3.5f,
        .pitch = 0.25f
    }));

    return run_example(scene.get(), camera.get(), {
        .window_title = "Instanced Mesh",
    });
}
