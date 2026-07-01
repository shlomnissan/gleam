/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <string>

#include <vglx/vglx.hpp>

constexpr int kWindowWidth = 1024;
constexpr int kWindowHeight = 768;
constexpr int kSampleCount = 4;

struct ExampleSettings {
    std::string window_title = {"Example"};
    vglx::Color clear_color = {0x000000};
    vglx::Renderer::ToneMapping tone_mapping {vglx::Renderer::ToneMapping::None};
    float exposure {1.0f};
    vglx::Renderer::ShadowMap shadow_map {vglx::Renderer::ShadowMap::None};
};

auto run_example(vglx::Scene* scene, vglx::Camera* camera, const ExampleSettings& settings) -> int;
