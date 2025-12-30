/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/loaders/load_handle.hpp"

#include <filesystem>
#include <memory>
#include <string>

namespace vglx {

namespace fs = std::filesystem;

class LoadScheduler {
public:
    LoadScheduler();

    LoadScheduler(const LoadScheduler&) = delete;
    auto operator=(const LoadScheduler&) -> LoadScheduler& = delete;

    LoadScheduler(LoadScheduler&&) noexcept = delete;
    auto operator=(LoadScheduler&&) noexcept -> LoadScheduler& = delete;

    [[nodiscard]] auto LoadTexture(const fs::path& path) -> TextureLoadHandle;

    [[nodiscard]] auto LoadMesh(const fs::path& path) -> MeshLoadHandle;

    auto Pump() -> void;

    ~LoadScheduler();

private:
    /// @cond INTERNAL
    struct Impl;
    std::unique_ptr<Impl> impl_;
    /// @endcond
};

}