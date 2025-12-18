/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <vglx/core.hpp>
#include <vglx/scene.hpp>

#include "example_scene.hpp"

#include <array>

class ExampleFrustumCulling : public ExampleScene {
public:
    ExampleFrustumCulling();

    auto OnAttached(vglx::SharedContextPointer context) -> void override;

    auto OnUpdate(float delta) -> void override;

private:
    std::array<vglx::Mesh*, 2500> boxes_;
};