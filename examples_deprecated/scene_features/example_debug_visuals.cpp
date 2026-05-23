/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "example_debug_visuals.hpp"

#include "ui_helpers.hpp"

#include <array>

using namespace vglx;

ExampleDebugVisuals::ExampleDebugVisuals(Camera* camera) {
    Add(OrbitControls::Create(camera, {
        .radius = 5.0f,
        .pitch = math::DegToRad(25.0f),
        .yaw = math::DegToRad(45.0f)
    }));

    arrows_ = Add(Node::Create());
    arrows_->Add(Arrow::Create({
        .direction = {0.5f, 0.0f, 0.0f},
        .origin = {0.0f, 0.0f, 0.0f},
        .color = 0xFF0000,
        .length = 1.2f
    }));

    arrows_->Add(Arrow::Create({
        .direction = {0.0f, 0.5f, 0.0f},
        .origin = {0.0f, 0.0f, 0.0f},
        .color = 0x00FF00,
        .length = 1.2f
    }));

    arrows_->Add(Arrow::Create({
        .direction = Cross(Vector3(0.5f, 0.0f, 0.0f), Vector3(0.0f, 0.5f, 0.0f)),
        .origin = {0.0f, 0.0f, 0.0f},
        .color = 0x0000FF,
        .length = 1.2f
    }));
    arrows_->TranslateY(0.1f);

    grid_ptr_ = Grid::Create(grid_params_);
}

auto ExampleDebugVisuals::OnUpdate(float _) -> void {
    if (curr_visual_ == "arrows" && !IsChild(arrows_)) {
        if (IsChild(grid_)) {
            grid_ptr_ = Detach(grid_);
        }
        arrows_ = Add(std::move(arrows_ptr_));
    }

    if (curr_visual_ == "grid" && !IsChild(grid_)) {
        if (IsChild(arrows_)) {
            arrows_ptr_ = Detach(arrows_);
        }
        grid_ = Add(std::move(grid_ptr_));
    }
}

auto ExampleDebugVisuals::ContextMenu() -> void {
    static bool dirty = false;
    static auto visuals = std::array<const char*, 2> {"arrows", "grid"};

    UIDropDown("visuals", visuals, curr_visual_,
        [this](std::string_view str) { curr_visual_ = str; }
    );

    if (curr_visual_ == "grid") {
        UIColor("color", &grid_params_.color[0], dirty, "color");
        UISliderFloat("size", grid_params_.size, 1.0, 10.0f, dirty);
        UISliderUnsigned("divisions", grid_params_.divisions, 1, 64, dirty);

        if (dirty) {
            dirty = false;
            Remove(grid_);
            grid_ = Add(Grid::Create(grid_params_));
        }
    }
}