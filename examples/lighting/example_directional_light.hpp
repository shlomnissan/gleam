/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "example_scene.hpp"

#include <memory>

#include <vglx/vglx.hpp>

class ExampleDirectionalLight : public ExampleScene {
public:
    ExampleDirectionalLight(vglx::Camera* camera);

    auto ContextMenu() -> void override;

private:
    vglx::DirectionalLight* directional_light_ {nullptr};

    std::shared_ptr<vglx::PhongMaterial> phong_material_;
};