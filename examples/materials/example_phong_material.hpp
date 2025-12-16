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
    ExamplePhongMaterial();

    auto OnAttached(vglx::SharedContextPointer context) -> void override;

    auto OnUpdate(float delta) -> void override;

    auto ContextMenu() -> void override;

private:
    std::shared_ptr<vglx::PhongMaterial> material_;
    std::shared_ptr<vglx::Mesh> mesh_;
    std::shared_ptr<vglx::Texture2D> texture_;
};