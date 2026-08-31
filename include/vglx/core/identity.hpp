/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include <string>
#include <string_view>

namespace vglx {

/// @cond INTERNAL
class VGLX_EXPORT Identity {
public:
    Identity();

    [[nodiscard]] const auto& UUID() const { return uuid_; }

    [[nodiscard]] const auto& Name() const { return name_; }

    [[nodiscard]] const auto& DisplayName() const {
        return name_.empty() ? uuid_ : name_;
    }

    auto SetName(std::string_view name) { name_ = name; }

private:
    std::string uuid_;

    std::string name_ {};
};
/// @endcond

}
