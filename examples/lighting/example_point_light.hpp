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

class ExamplePointLight : public ExampleScene {
public:
    ExamplePointLight(vglx::Camera* camera);

    auto ContextMenu() -> void override;

private:
    vglx::PointLight* point_light_ {nullptr};

    std::shared_ptr<vglx::PhongMaterial> phong_material_;
};