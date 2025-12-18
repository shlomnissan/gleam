/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "example_scene.hpp"

#include <memory>

#include <vglx/lights.hpp>
#include <vglx/materials.hpp>

class ExamplePointLight : public ExampleScene {
public:
    ExamplePointLight();

    auto OnAttached(vglx::SharedContextPointer context) -> void override;

    auto ContextMenu() -> void override;

private:
    vglx::PointLight* point_light_ {nullptr};

    std::shared_ptr<vglx::PhongMaterial> phong_material_;
};