/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "loaders/texture_loader_xyz.hpp"

#include "vglx/asset_format.hpp"
#include "vglx/textures/texture_2d.hpp"

#include "utilities/file.hpp"

#include <cstdint>
#include <cstring>
#include <format>
#include <fstream>
#include <vector>

namespace vglx {

auto load_texture(const fs::path& path) -> std::expected<std::shared_ptr<Texture2D>, std::string> {
    auto file = std::ifstream {path, std::ios::binary};
    if (!file) {
        return std::unexpected(std::format("Unable to open image '{}'", path.string()));
    }

    auto header = TextureHeader {};
    read_binary(file, header);
    if (std::memcmp(header.magic, "TEX0", 4) != 0) {
        return std::unexpected(std::format("Invalid texture file '{}'", path.string()));
    }

    if (header.version != VGLX_TEX_VER) {
        return std::unexpected(std::format("Unsupported file version '{}'", path.string()));
    }

    auto data = std::vector<uint8_t>(header.pixel_data_size);
    if (!read_binary(file, data, header.pixel_data_size)) {
        return std::unexpected(std::format("Failed to read data from '{}'", path.string()));
    }

    auto out = std::make_shared<Texture2D>(Texture2D::Parameters {
        .width = header.width,
        .height = header.height,
        .data = std::move(data)
    });

    out->SetName(path.filename().string());

    return out;
}

}