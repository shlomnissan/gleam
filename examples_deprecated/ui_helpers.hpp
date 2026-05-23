/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <functional>
#include <span>
#include <string>
#include <string_view>

using Label = const std::string&;

auto UICheckbox(Label label, bool& value, bool& dirty) -> void;

auto UIColor(Label label, float* color, bool& dirty, Label id = "") -> void;

auto UIDropDown(
    const std::string& label,
    const std::span<const char*> items,
    const std::string_view selected_value,
    const std::function<void(std::string_view)>& callback
) -> void;

auto UISeparator() -> void;

auto UISliderFloat(
    Label label,
    float& value,
    float min,
    float max,
    bool& dirty,
    float width = 120.0f
) -> void;

auto UISliderUnsigned(
    Label label,
    unsigned& value,
    unsigned min,
    unsigned max,
    bool& dirty,
    float width = 120.0f
) -> void;

auto UIText(Label text) -> void;

auto Theme() -> void;