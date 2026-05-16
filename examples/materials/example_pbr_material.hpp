/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <vglx/materials.hpp>
#include <vglx/textures.hpp>

#include <memory>

#include "example_scene.hpp"

class ExamplePBRMaterial : public ExampleScene {
public:
    ExamplePBRMaterial(vglx::Camera* camera);

    auto OnUpdate(float delta) -> void override;

    auto ContextMenu() -> void override;

private:
    vglx::Mesh* mesh_ {nullptr};

    std::shared_ptr<vglx::PhongMaterial> material_;
    std::shared_ptr<vglx::Texture2D> texture_;

    float metallic_ = 0.0f;
    float roughness_ = 1.0f;
    float ao_intensity_ = 1.0f;
};
