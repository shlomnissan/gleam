/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <string>
#include <memory>

#include <vglx/vglx.hpp>

constexpr int kWindowWidth = 1024;
constexpr int kWindowHeight = 768;
constexpr int kSampleCount = 4;

struct ExampleScene : public vglx::Scene {
    static auto Create() {
        return std::make_unique<ExampleScene>();
    }

    virtual auto OnDrawUI() -> void {}
};

struct ExampleSettings {
    std::string window_title = {"Example"};
    vglx::Color clear_color = {0x000000};
    vglx::Renderer::ShadowMap shadow_map {vglx::Renderer::ShadowMap::None};
    vglx::Renderer::ToneMapping tone_mapping {vglx::Renderer::ToneMapping::None};
    float exposure {1.0f};
};

auto run_example(ExampleScene* scene, vglx::Camera* camera, const ExampleSettings& settings) -> int;
