/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/loaders.hpp"

#include "vglx/textures/texture_2d.hpp"

#include "loaders/detail/image_import.hpp"

namespace vglx {

auto LoadTexture(
    const fs::path& path,
    Texture::ColorSpace color_space
) -> std::expected<std::shared_ptr<Texture2D>, std::string> {
    auto result = detail::image::import(path);
    if (!result.has_value()) {
        return std::unexpected(result.error());
    }

    auto out = Texture2D::Create(result.value());
    out->generate_mipamps = true;
    out->min_filter = Texture::MinFilter::LinearMipmapLinear;
    out->mag_filter = Texture::MagFilter::Linear;
    out->color_space = color_space;
    out->SetName(path.filename().string());
    return out;
}

}