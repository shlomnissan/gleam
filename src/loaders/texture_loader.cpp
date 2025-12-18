/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/asset_format.hpp"
#include "vglx/loaders/texture_loader.hpp"

#include "utilities/file.hpp"

#include <cstring>
#include <fstream>

namespace vglx {

namespace {

auto load_texture(const fs::path& path, std::ifstream& file, const TextureHeader& h) {
    auto data = std::vector<uint8_t>(h.pixel_data_size);
    read_binary(file, data, h.pixel_data_size);

    auto texture = std::make_shared<Texture2D>(Texture2D::Parameters {
        .width = h.width,
        .height = h.height,
        .data = std::move(data)
    });

    texture->SetName(path.filename().string());
    return texture;
}

}

auto TextureLoader::LoadImpl(const fs::path& path) const -> LoaderResult<std::shared_ptr<Texture2D>> {
    auto file = std::ifstream {path, std::ios::binary};
    auto path_s = path.string();
    if (!file) {
        return std::unexpected("Unable to open file '" + path_s + "'");
    }

    auto texture_header = TextureHeader {};
    read_binary(file, texture_header);
    if (std::memcmp(texture_header.magic, "TEX0", 4) != 0) {
        return std::unexpected("Invalid texture file '" + path_s + "'");
    }

    if (texture_header.version != VGLX_TEX_VER) {
        std::unexpected("Unsupported texture version in file '" + path_s + "'");
    }

    return load_texture(path, file, texture_header);
}

}