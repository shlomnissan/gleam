/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/core/asset_manager.hpp"

namespace vglx {

struct AssetManager::Impl {};

AssetManager::AssetManager() : impl_(std::make_unique<Impl>()) {}

auto AssetManager::Pump() -> void {}

AssetManager::~AssetManager() = default;

}