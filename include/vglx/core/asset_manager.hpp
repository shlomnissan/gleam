/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include <memory>

namespace vglx {

class VGLX_EXPORT AssetManager {
public:
    AssetManager();

    AssetManager(const AssetManager&) = delete;
    auto operator=(const AssetManager&) -> AssetManager& = delete;

    AssetManager(AssetManager&&) noexcept = default;
    auto operator=(AssetManager&&) noexcept -> AssetManager& = default;

    auto Pump() -> void;

    ~AssetManager();

private:
    /// @cond INTERNAL
    struct Impl;
    std::unique_ptr<Impl> impl_;
    /// @endcond
};

}