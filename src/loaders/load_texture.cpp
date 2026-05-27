/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/loaders.hpp"

#include "vglx/textures/texture_2d.hpp"

namespace vglx {

auto LoadTexture(
    const fs::path& path,
    Texture::ColorSpace color_space
) -> std::expected<std::shared_ptr<Texture2D>, std::string> {
    auto image = LoadImage(path);
    if (!image.has_value()) {
        return std::unexpected(image.error());
    }

    auto out = Texture2D::Create(image.value());
    out->generate_mipamps = false;
    out->min_filter = Texture::MinFilter::Linear;
    out->mag_filter = Texture::MagFilter::Linear;
    out->color_space = color_space;
    out->SetName(path.filename().string());
    return out;
}

auto LoadHDRTexture(
    const fs::path& path
) -> std::expected<std::shared_ptr<Texture2D>, std::string> {
    auto image = LoadHDRImage(path);
    if (!image.has_value()) {
        return std::unexpected(image.error());
    }

    auto out = Texture2D::Create(image.value());
    out->generate_mipamps = false;
    out->min_filter = Texture::MinFilter::Linear;
    out->mag_filter = Texture::MagFilter::Linear;
    out->color_space = Texture::ColorSpace::Linear;
    out->SetName(path.filename().string());
    return out;
}

}
