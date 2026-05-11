/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <vglx/materials.hpp>

#include <array>
#include <memory>

#include "example_scene.hpp"

class ExampleBlending : public ExampleScene {
public:
    ExampleBlending(vglx::Camera* camera);

    auto ContextMenu() -> void override;

private:
    std::shared_ptr<vglx::PhongMaterial> transparent_material_;
};