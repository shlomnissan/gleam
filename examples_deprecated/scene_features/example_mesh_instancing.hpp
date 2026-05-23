/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <vglx/vglx.hpp>

#include "example_scene.hpp"

class ExampleMeshInstancing : public ExampleScene {
public:
    ExampleMeshInstancing(vglx::Camera* camera);

    auto OnUpdate(float delta) -> void override;

private:
    vglx::InstancedMesh* boxes_ {nullptr};
};