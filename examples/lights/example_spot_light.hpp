/*
===========================================================================
  GLEAM ENGINE https://gleamengine.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "example_scene.hpp"

#include <memory>

#include <gleam/lights.hpp>
#include <gleam/materials.hpp>

class ExampleSpotLight : public ExampleScene {
public:
    explicit ExampleSpotLight(std::shared_ptr<gleam::Camera>);

    auto ContextMenu() -> void override;

private:
    std::shared_ptr<gleam::PhongMaterial> phong_material_;
    std::shared_ptr<gleam::SpotLight> spot_light_;
};