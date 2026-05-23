/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <memory>
#include <string>

#include <vglx/vglx.hpp>

#include "example_scene.hpp"

class ExampleDebugVisuals : public ExampleScene {
public:
    ExampleDebugVisuals(vglx::Camera* camera);

    auto OnUpdate(float delta) -> void override;

    auto ContextMenu() -> void override;

private:
    std::string curr_visual_ {"arrows"};

    vglx::Grid::Parameters grid_params_ {
        .color = vglx::Color(0x333333),
        .size = 4.0f,
        .divisions = 16
    };

    vglx::Node* arrows_ {nullptr};
    std::unique_ptr<vglx::Node> arrows_ptr_ {};

    vglx::Grid* grid_ {nullptr};
    std::unique_ptr<vglx::Grid> grid_ptr_ {};
};