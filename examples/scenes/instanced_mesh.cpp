/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <array>
#include <cmath>
#include <memory>
#include <print>
#include <random>

#include "example_runner.hpp"

constexpr auto colors = std::array<vglx::Color, 3> {0x00FFFF, 0xFFFF00, 0xFF00FF};
constexpr auto count = 100;

auto rng {std::mt19937 {0u}};
auto dist {std::uniform_real_distribution<float> {0.0f, 1.0f}};
auto elapsed {0.0f};
auto camera_target {vglx::Vector3::Zero()};
auto seeds {std::vector<float>(count * count)};

struct Scene : public ExampleScene {
    vglx::InstancedMesh* mesh {nullptr};
    vglx::PerspectiveCamera* camera {nullptr};

    Scene(vglx::PerspectiveCamera* camera) : camera {camera} {
        auto path = ASSETS_DIR "/textures/instance.jpg";
        auto texture = vglx::LoadTexture(path);
        if (!texture.has_value()) {
            std::println(stderr, "Failed to load texture {}", path);
        }

        mesh = this->Add(vglx::InstancedMesh::Create(
            vglx::BoxGeometry::Create(),
            vglx::UnlitMaterial::Create({
                .color = 0x00FFFF,
                .texture_map = texture.has_value() ? texture.value() : nullptr
            }),
            count * count
        ));

        auto offset = static_cast<float>(count - 1) / 2.0f;
        for (auto x = 0, i = 0; x < count; ++x) {
            for (auto z = 0; z < count; ++z) {
                auto t = vglx::Transform3 {};
                t.SetPosition({offset - static_cast<float>(x), 0.0f, offset - static_cast<float>(z)});
                mesh->SetTransformAt(i, t.Get());

                auto l = 0.5f + dist(rng) * 0.5f;
                auto d = dist(rng) * (1.0f - l);
                mesh->SetColorAt(i, colors.front() * vglx::Color {l + d, l - d, l - d});

                seeds[i] = dist(rng);

                i++;
            }
        }
    }

    auto OnUpdate(float dt) -> void override {
        elapsed += dt * 0.5f;

        auto pos_x = vglx::math::Sin(elapsed / 4.0f) * 10.0f;
        auto pos_y = 8.0f + vglx::math::Cos(elapsed / 2.0f) * 2.0f;
        auto pos_z = vglx::math::Cos(elapsed / 4.0f) * 10.0f;

        camera_target.x = vglx::math::Sin(elapsed) * -8.0f;
        camera_target.y = vglx::math::Cos(elapsed) * -8.0f;

        camera->up.x = vglx::math::Sin(elapsed / 400);
        camera->transform.SetPosition({pos_x, pos_y, pos_z});
        camera->LookAt(camera_target);

        for (auto i = 0; i < count * count; ++i) {
            auto transform = vglx::Transform3 {mesh->TransformAt(i)};
            auto offset = std::abs(vglx::math::Sin((elapsed + seeds[i]) * 5.0f + seeds[i]));
            transform.position.y = offset;
            mesh->SetTransformAt(i, transform.Get());
        }
    }
};

auto get_camera() {
    auto camera = vglx::PerspectiveCamera::Create({
        .fov = vglx::math::DegToRad(60.0f),
        .aspect = static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight),
        .near = 0.3f,
        .far = 1000.0f
    });

    camera->transform.SetPosition({10.0f, 10.0f, 10.0f});

    return camera;
}

auto main() -> int {
    auto camera = get_camera();
    auto scene = std::make_unique<Scene>(camera.get());

    return run_example(scene.get(), camera.get(), {
        .window_title = "Instanced Mesh",
    });
}
