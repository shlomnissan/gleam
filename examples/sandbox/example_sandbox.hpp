/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <memory>

#include "example_scene.hpp"

class ExampleSandbox : public ExampleScene {
public:
    ExampleSandbox();

    auto OnAttached(vglx::SharedContextPointer context) -> void override;

    auto OnUpdate([[maybe_unused]] float delta) -> void override;

private:
    std::shared_ptr<vglx::Node> model_;
};