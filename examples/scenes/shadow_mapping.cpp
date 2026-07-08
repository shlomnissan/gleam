/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <memory>

#include "example_runner.hpp"

struct Scene : public vglx::Scene {
    vglx::Mesh* mesh {nullptr};

    Scene() {
        fog = vglx::Fog::CreateExponential(vglx::Color {0x222244}, 0.2f);
        AddGeometry();
        AddLights();
    }

    auto AddGeometry() -> void {
        mesh = this->Add(vglx::Mesh::Create(
            vglx::TorusKnotGeometry::Create({
                .radius = 1.2f,
                .tubular_segments = 128,
                .radial_segments = 64
            }),
            vglx::PhongMaterial::Create({.color = 0x999999})
        ));

        mesh->cast_shadow = true;
        mesh->receive_shadow = true;
        mesh->transform.SetPosition({0.0f, 0.8f, 0.0f});
        mesh->transform.SetScale({0.3f, 0.3f, 0.3f});

        auto plane = Add(vglx::Mesh::Create(
            vglx::PlaneGeometry::Create({.width = 200, .height = 200}),
            vglx::PhongMaterial::Create({.color = 0x999999})
        ));

        plane->receive_shadow = true;
        plane->transform.Rotate(vglx::Vector3::Right(), -vglx::math::pi_over_2);
    }

    auto AddLights() -> void {
        Add(vglx::AmbientLight::Create({.color = 0x444444, .intensity = 0.7f}));

        auto spot = Add(vglx::SpotLight::Create({
            .color = 0xff8888,
            .intensity = 400.0f,
            .angle = 0.20f,
            .penumbra = 0.3f,
        }));

        spot->transform.Translate({8.0f, 10.0f, 5.0f});
        spot->cast_shadow = true;
        spot->target = mesh;
        spot->shadow.map_size = 256;
        spot->shadow.bias = 0.002f;
        spot->shadow.far = 20.0f;

        auto directional = Add(vglx::DirectionalLight::Create({
            .color = 0x8888ff,
            .intensity = 3.0f
        }));

        directional->transform.Translate({3.0f, 12.0f, 17.0f});
    }

    auto OnUpdate(float dt) -> void override {
        mesh->transform.Rotate(vglx::Vector3::Right(), 0.25f * dt);
        mesh->transform.Rotate(vglx::Vector3::Up(), 0.5f * dt);
        mesh->transform.Rotate(vglx::Vector3::Forward(), 1.0f * dt);
    }
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
        .target = scene->mesh->transform.position,
        .radius = 4.0f,
        .pitch = 0.25f
    }));

    return run_example(scene.get(), camera.get(), {
        .window_title = "Shadow Mapping",
        .clear_color = 0x222244,
        .shadow_map = vglx::Renderer::ShadowMap::Basic
    });
}
