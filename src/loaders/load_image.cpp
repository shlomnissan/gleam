/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/loaders.hpp"

#include "loaders/detail/image_import.hpp"

#include <algorithm>
#include <array>
#include <format>
#include <memory>

namespace vglx {

namespace {

const std::array<std::string, 6> exts {".png", ".jpg", ".jpeg", ".tga", ".bmp", ".hdr"};

}

auto LoadImage(
    const fs::path& path
) -> std::expected<std::shared_ptr<Image>, std::string> {
    if (!fs::exists(path)) {
        return std::unexpected(std::format("Can't find image {}", path.string()));
    }

    auto ext = path.extension().string();
    if (std::ranges::find(exts, ext) == exts.end()) {
        return std::unexpected(std::format("Unsupported file extension {}", ext));
    }

    return detail::image::import(path);
}

}