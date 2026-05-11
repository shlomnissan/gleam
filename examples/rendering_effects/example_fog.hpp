/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <memory>

#include "example_scene.hpp"

class ExampleFog : public ExampleScene {
public:
    ExampleFog(vglx::Camera* camera);

    auto ContextMenu() -> void override;
};