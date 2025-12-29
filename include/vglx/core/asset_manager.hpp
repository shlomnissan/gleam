/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/core/asset_handle.hpp"

#include <filesystem>
#include <memory>
#include <string>

namespace vglx {

namespace fs = std::filesystem;

class VGLX_EXPORT AssetManager {
public:
    AssetManager();

    AssetManager(const AssetManager&) = delete;
    auto operator=(const AssetManager&) -> AssetManager& = delete;

    AssetManager(AssetManager&&) noexcept = delete;
    auto operator=(AssetManager&&) noexcept -> AssetManager& = delete;

    [[nodiscard]] auto LoadTexture(const fs::path& path) -> TextureHandle;

    [[nodiscard]] auto LoadMesh(const fs::path& path) -> MeshHandle;

    auto Pump() -> void;

    ~AssetManager();

private:
    /// @cond INTERNAL
    struct Impl;
    std::unique_ptr<Impl> impl_;
    /// @endcond
};

}