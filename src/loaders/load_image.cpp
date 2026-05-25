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

const std::array<std::string, 5> ldr_exts {".png", ".jpg", ".jpeg", ".tga", ".bmp"};
const std::array<std::string, 1> hdr_exts {".hdr"};

}

auto LoadImage(
    const fs::path& path
) -> std::expected<std::shared_ptr<Image>, std::string> {
    if (!fs::exists(path)) {
        return std::unexpected(std::format("Can't find image {}", path.string()));
    }

    auto ext = path.extension().string();
    if (std::ranges::find(ldr_exts, ext) == ldr_exts.end()) {
        return std::unexpected(std::format("Unsupported file extension {}", ext));
    }

    if (detail::image::is_hdr(path)) {
        return std::unexpected("HDR images must be loaded with LoadHDRImage(...)");
    }

    return detail::image::import(path);
}

auto LoadHDRImage(
    const fs::path& path
) -> std::expected<std::shared_ptr<Image>, std::string> {
    if (!fs::exists(path)) {
        return std::unexpected(std::format("Can't find image {}", path.string()));
    }

    auto ext = path.extension().string();
    if (std::ranges::find(hdr_exts, ext) == hdr_exts.end()) {
        return std::unexpected(std::format("Unsupported file extension {}", ext));
    }

    if (!detail::image::is_hdr(path)) {
        return std::unexpected("Non-HDR images must be loaded with LoadImage(...)");
    }

    return detail::image::import_hdr(path);
}

}