/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "loaders/detail/texture_import.hpp"

#include "vglx/asset_format.hpp"
#include "vglx/textures/image.hpp"
#include "vglx/textures/texture_2d.hpp"

#include "loaders/detail/image_import.hpp"
#include "utilities/file.hpp"

#include <cstdint>
#include <cstring>
#include <format>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace vglx::detail::texture {

struct ImageResult {
    std::shared_ptr<Image> image;
    Texture::ColorSpace color_space {Texture::ColorSpace::sRGB};
};

namespace {

auto image_from_raw_image_source(const fs::path& path) -> std::expected<ImageResult, std::string> {
    auto image = image::import(path);
    if (!image) return std::unexpected(image.error());
    return ImageResult {.image = std::move(image.value())};
}

auto image_from_engine_image_source(const fs::path& path) -> std::expected<ImageResult, std::string> {
    auto file = std::ifstream {path, std::ios::binary};
    if (!file) {
        return std::unexpected(std::format("Unable to open texture '{}'", path.string()));
    }

    auto header = TextureHeader {};
    if (!read_binary(file, header)) {
        return std::unexpected(std::format("Failed to read header from '{}'", path.string()));
    }

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

    auto color_space = header.color_space == TextureColorSpace_Linear
        ? Texture::ColorSpace::Linear
        : Texture::ColorSpace::sRGB;

    return ImageResult {
        .image = Image::Create({
            .data = std::move(data),
            .width = header.width,
            .height = header.height,
        }),
        .color_space = color_space
    };
}

}

auto import(const fs::path& path) -> std::expected<std::shared_ptr<Texture2D>, std::string> {
    auto result = path.extension().string() == ".tex"
        ? image_from_engine_image_source(path)
        : image_from_raw_image_source(path);

    if (!result.has_value()) return std::unexpected(result.error());

    auto out = Texture2D::Create(result->image);
    out->color_space = result->color_space;
    out->generate_mipamps = true;
    out->min_filter = Texture::MinFilter::LinearMipmapLinear;
    out->mag_filter = Texture::MagFilter::Linear;
    out->SetName(path.filename().string());

    return out;
}

}