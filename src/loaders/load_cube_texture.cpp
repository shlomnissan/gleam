/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/loaders.hpp"

#include "vglx/textures/cube_texture.hpp"

#include "loaders/detail/image_import.hpp"

namespace vglx {

auto LoadCubeTexture(
    const CubeTexturePaths& paths,
    Texture::ColorSpace color_space
) -> std::expected<std::shared_ptr<CubeTexture>, std::string> {
    auto px = detail::image::import(paths.positive_x, false);
    if (!px.has_value()) return std::unexpected(px.error());

    auto nx = detail::image::import(paths.negative_x, false);
    if (!nx.has_value()) return std::unexpected(nx.error());

    auto py = detail::image::import(paths.positive_y, false);
    if (!py.has_value()) return std::unexpected(py.error());

    auto ny = detail::image::import(paths.negative_y, false);
    if (!ny.has_value()) return std::unexpected(ny.error());

    auto pz = detail::image::import(paths.positive_z, false);
    if (!pz.has_value()) return std::unexpected(pz.error());

    auto nz = detail::image::import(paths.negative_z, false);
    if (!nz.has_value()) return std::unexpected(nz.error());

    auto out = CubeTexture::Create({
        .positive_x = std::move(px.value()),
        .negative_x = std::move(nx.value()),
        .positive_y = std::move(py.value()),
        .negative_y = std::move(ny.value()),
        .positive_z = std::move(pz.value()),
        .negative_z = std::move(nz.value()),
    });

    out->min_filter = Texture::MinFilter::Linear;
    out->mag_filter = Texture::MagFilter::Linear;
    out->color_space = color_space;

    return out;
}

}