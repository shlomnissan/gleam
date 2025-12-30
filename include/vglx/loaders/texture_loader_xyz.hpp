/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include <expected>
#include <filesystem>
#include <memory>
#include <string>

#include "vglx/loaders/load_handle.hpp"

namespace vglx {

class Texture2D;
class LoadScheduler;

namespace fs = std::filesystem;

auto load_texture(const fs::path& path) -> std::expected<std::shared_ptr<Texture2D>, std::string>;

class VGLX_EXPORT TextureLoaderXYZ {
public:
    explicit TextureLoaderXYZ(LoadScheduler* scheduler);

    auto Load(const fs::path& path) -> std::expected<std::shared_ptr<Texture2D>, std::string>;

    auto LoadAsync(const fs::path& path) -> TextureLoadHandle;

private:
    LoadScheduler* load_scheduler_ {nullptr};
};

}