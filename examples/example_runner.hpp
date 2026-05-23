/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <string_view>

#include <vglx/vglx.hpp>

constexpr int kWindowWidth = 1024;
constexpr int kWindowHeight = 768;
constexpr int kSampleCount = 4;

auto RunExample(vglx::Scene* scene, vglx::Camera* camera, std::string_view window_title) -> int;