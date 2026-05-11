/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <vglx/materials.hpp>
#include <vglx/scene.hpp>
#include <vglx/textures.hpp>

#include <memory>

#include "example_scene.hpp"

class ExamplePhongMaterial : public ExampleScene {
public:
    ExamplePhongMaterial(vglx::Camera* camera);

    auto OnUpdate(float delta) -> void override;

    auto ContextMenu() -> void override;

private:
    vglx::Mesh* mesh_ {nullptr};

    std::shared_ptr<vglx::PhongMaterial> material_;
    std::shared_ptr<vglx::Texture2D> texture_;
};