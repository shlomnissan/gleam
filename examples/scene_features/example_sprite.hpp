/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "example_scene.hpp"

#include "vglx/scene/sprite.hpp"

#include <memory>

class ExampleSprite : public ExampleScene {
public:
    ExampleSprite();

    auto OnAttached(vglx::SharedContextPointer context) -> void override;

    auto OnUpdate(float delta) -> void override;

private:
    std::shared_ptr<vglx::Sprite> sprite_;
};