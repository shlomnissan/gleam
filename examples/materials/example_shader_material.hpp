/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <vglx/core.hpp>
#include <vglx/materials.hpp>
#include <vglx/scene.hpp>
#include <vglx/utilities.hpp>

#include <memory>

#include "example_scene.hpp"

class ExampleShaderMaterial : public ExampleScene {
public:
    ExampleShaderMaterial(vglx::Camera* camera);

    auto OnUpdate(float delta) -> void override;

    auto ContextMenu() -> void override;

private:
    vglx::Timer timer_ {true};

    vglx::Mesh* mesh_ {nullptr};

    std::shared_ptr<vglx::ShaderMaterial> material_;
};