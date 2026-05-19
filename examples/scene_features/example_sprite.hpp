/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "example_scene.hpp"

#include <vglx/vglx.hpp>

#include <memory>

class ExampleSprite : public ExampleScene {
public:
    ExampleSprite(vglx::Camera* camera);

    auto OnUpdate(float delta) -> void override;

private:
    vglx::Sprite* sprite_ {nullptr};
};