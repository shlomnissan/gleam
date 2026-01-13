/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <vglx/materials/material.hpp>
#include <vglx/math/color.hpp>

#include <memory>
#include <unordered_map>

namespace vglx {

class GLState {
public:
    auto ProcessMaterial(const Material* material) -> void;

    auto SetClearColor(const Color& color) -> void;

    auto SetDepthMask(bool enabled) -> void;

    auto SetViewport(int x, int y, int width, int height) const -> void;

    auto UseProgram(unsigned int program_id) -> void;

    auto Reset() -> void;

private:
    std::unordered_map<int, bool> features_;

    Material::Blending curr_blending_ {Material::Blending::None};

    Color curr_clear_color_ {0.0f, 0.0f, 0.0f};

    bool curr_backface_culling_ {false};

    bool curr_depth_mask_ {false};

    unsigned int curr_program_ = 0;

    auto Enable(int token) -> void;

    auto Disable(int token) -> void;

    auto SetBackfaceCulling(bool enabled) -> void;

    auto SetDepthTest(bool enabled) -> void;

    auto SetPolygonOffset(float factor, float units) -> void;

    auto SetBlending(Material::Blending blending) -> void;
};

}