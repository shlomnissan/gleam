/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================

Based on three.js dynamic instancing example
https://threejs.org/examples/?q=instan#webgl_instancing_dynamic

*/

#include <array>
#include <cmath>
#include <memory>
#include <print>
#include <random>
#include <vector>

#include "example_runner.hpp"

constexpr auto colors = std::array<vglx::Color, 3> {0x00FFFFu, 0xFFFF00u, 0xFF00FFu};
constexpr auto row_count = 100;
constexpr auto total_count = row_count * row_count;
constexpr auto row_offset = static_cast<float>(row_count - 1) / 2.0f;
constexpr auto max_distance = vglx::math::Sqrt(2.0f) * row_offset + 1.0f;

auto rng {std::mt19937 {0u}};
auto dist {std::uniform_real_distribution<float> {0.0f, 1.0f}};
auto elapsed {0.0f};
auto camera_target {vglx::Vector3::Zero()};
auto cycle_time = 0.0f;
auto curr_color_idx = 0;
auto next_color_idx = 0;

auto seeds {std::vector<float>(total_count)};
auto tints {std::vector<vglx::Color>(total_count)};
auto transforms {std::vector<vglx::Transform3>(total_count)};

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
                .color = 0xFFFFFFu,
                .texture_map = texture.has_value() ? texture.value() : nullptr
            }),
            total_count
        ));

        mesh->frustum_culled = false;

        for (auto x = 0, i = 0; x < row_count; ++x) {
            for (auto z = 0; z < row_count; ++z) {
                transforms[i].SetPosition({row_offset - static_cast<float>(x), 0.0f, row_offset - static_cast<float>(z)});
                mesh->SetTransformAt(i, transforms[i].Get());

                auto l = 0.5f + dist(rng) * 0.5f;
                auto d = dist(rng) * (1.0f - l);
                tints[i] = vglx::Color {l + d, l - d, l - d};
                seeds[i] = dist(rng);

                mesh->SetColorAt(i, colors.front() * tints[i]);

                i++;
            }
        }
    }

    auto OnUpdate(float dt) -> void override {
        elapsed += dt * 0.5f;
        cycle_time += dt / 2.0f;

        if (cycle_time >= 1.0f) {
            cycle_time = 0.0f;
            curr_color_idx = next_color_idx;
            next_color_idx = (next_color_idx + 1) % colors.size();
        }

        auto pos_x = vglx::math::Sin(elapsed / 4.0f) * 10.0f;
        auto pos_y = 8.0f + vglx::math::Cos(elapsed / 2.0f) * 2.0f;
        auto pos_z = vglx::math::Cos(elapsed / 4.0f) * 10.0f;

        camera_target.x = vglx::math::Sin(elapsed) * -8.0f;
        camera_target.y = vglx::math::Cos(elapsed) * -8.0f;

        camera->up.x = vglx::math::Sin(elapsed / 400);
        camera->transform.SetPosition({pos_x, pos_y, pos_z});
        camera->LookAt(camera_target);

        for (auto i = 0; i < total_count; ++i) {
            auto offset = std::abs(vglx::math::Sin((elapsed + seeds[i]) * 4.0f + seeds[i]));
            transforms[i].position.y = offset;
            transforms[i].touched = true;
            mesh->SetTransformAt(i, transforms[i].Get());

            if (cycle_time < 1.0f) {
                auto distance = transforms[i].position.Length() / max_distance;
                if (distance <= cycle_time) {
                    mesh->SetColorAt(i, colors[next_color_idx] * tints[i]);
                }
            }
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
        .clear_color = 0xADD8E6u
    });
}
