/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "example_scene.hpp"

#include <vglx/core.hpp>
#include <vglx/scene.hpp>

#include <memory>

class ExampleSandbox : public ExampleScene {
public:
    ExampleSandbox();

    auto OnAttached(vglx::SharedContextPointer context) -> void override;

private:
    vglx::Node* mesh_ {nullptr};
};