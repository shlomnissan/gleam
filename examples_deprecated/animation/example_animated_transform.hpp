/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <vglx/vglx.hpp>

#include "example_scene.hpp"

#include <array>

class ExampleAnimatedTransform : public ExampleScene {
public:
    ExampleAnimatedTransform(vglx::Camera* camera);

    auto OnUpdate(float delta) -> void override;

private:
    vglx::Mesh* active_point_ {nullptr};

    std::shared_ptr<vglx::PhongMaterial> active_material_;

    vglx::Vector3 start_pos_ {-1.5f, 0.0f, 0.0f};
    vglx::Vector3 end_pos_ {1.5f, 0.0f, 0.0f};
    vglx::Vector3 start_scale_ {1.0f};
    vglx::Vector3 end_scale_ {0.5f};
    vglx::Color start_color_ {0xFE4A49};
    vglx::Color end_color_ {0xFED766};

    float elapsed_time_ {0.0f};
};