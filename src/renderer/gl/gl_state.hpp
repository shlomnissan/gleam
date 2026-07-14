/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <vglx/materials/material.hpp>
#include <vglx/math/color.hpp>

#include <unordered_map>

namespace vglx {

class GLState {
public:
    auto SetSide(Material::Side side) -> void;

    auto SetBlending(Material::Blending blending) -> void;

    auto SetDepthFunction(Material::Depth depth) -> void;

    auto SetClearColor(const Color& color) -> void;

    auto SetDepthTest(bool enabled) -> void;

    auto SetDepthWrites(bool enabled) -> void;

    auto SetPolygonOffset(float factor, float units) -> void;

    auto SetViewport(int x, int y, int width, int height) const -> void;

    auto SetSeamlessCubemapFiltering() -> void;

    auto ProcessMaterial(const Material* material) -> void;

    auto UseProgram(unsigned int program_id) -> void;

    auto Reset() -> void;

private:
    std::unordered_map<int, bool> features_;

    Material::Blending curr_blending_ {Material::Blending::None};

    Material::Depth curr_depth_ {Material::Depth::Less};

    Color curr_clear_color_ {0.0f, 0.0f, 0.0f};

    bool curr_cull_front_ {false};

    bool curr_depth_mask_ {true};

    unsigned int curr_program_ {0};

    auto Enable(int token) -> void;

    auto Disable(int token) -> void;
};

}
