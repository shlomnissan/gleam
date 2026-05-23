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

class ExampleSandbox : public ExampleScene {
public:
    ExampleSandbox(vglx::Camera* camera);

private:
    vglx::Node* mesh_ {nullptr};
};