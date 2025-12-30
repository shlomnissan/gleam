/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <expected>
#include <filesystem>
#include <memory>
#include <string>

#include "vglx/loaders/load_handle.hpp"

namespace vglx {

class Node;
class LoadScheduler;

namespace fs = std::filesystem;

class VGLX_EXPORT MeshLoaderXYZ {
public:
    explicit MeshLoaderXYZ(LoadScheduler* scheduler);

    auto Load(const fs::path& path) -> std::expected<std::shared_ptr<Node>, std::string>;

    auto LoadAsync(const fs::path& path) -> MeshLoadHandle;

private:
    LoadScheduler* load_scheduler_ {nullptr};
};

}