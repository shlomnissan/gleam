/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/math/utilities.hpp"

#include <string>
#include <string_view>

namespace vglx {

/// @cond INTERNAL
class Identity {
public:
    [[nodiscard]] const auto& UUID() const { return uuid_; }

    [[nodiscard]] const auto& Name() const { return name_; }

    [[nodiscard]] const auto& DisplayName() const {
        return name_.empty() ? uuid_ : name_;
    }

    auto SetName(std::string_view name) { name_ = name; }

private:
    std::string uuid_ {math::GenerateUUID()};

    std::string name_ {};
};
/// @endcond

}
