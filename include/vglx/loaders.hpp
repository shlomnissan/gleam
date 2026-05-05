/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include <expected>
#include <filesystem>
#include <memory>
#include <string>

#include "vglx/textures/texture.hpp"

namespace vglx {

namespace fs = std::filesystem;

class Image;
class Texture2D;
class CubeTexture;
class Node;

struct CubeTexturePaths {
    fs::path positive_x; ///< Right face (+X).
    fs::path negative_x; ///< Left face (-X).
    fs::path positive_y; ///< Top face (+Y).
    fs::path negative_y; ///< Bottom face (-Y).
    fs::path positive_z; ///< Front face (+Z).
    fs::path negative_z; ///< Back face (-Z).
};

[[nodiscard]] VGLX_EXPORT auto LoadImage(
    const fs::path& path
) -> std::expected<std::shared_ptr<Image>, std::string>;

[[nodiscard]] VGLX_EXPORT auto LoadTexture(
    const fs::path& path,
    Texture::ColorSpace color_space = Texture::ColorSpace::sRGB
) -> std::expected<std::shared_ptr<Texture2D>, std::string>;

[[nodiscard]] VGLX_EXPORT auto LoadCubeTexture(
    const CubeTexturePaths& paths,
    Texture::ColorSpace color_space = Texture::ColorSpace::sRGB
) -> std::expected<std::shared_ptr<CubeTexture>, std::string>;

[[nodiscard]] VGLX_EXPORT auto LoadMesh(
    const fs::path& path
) -> std::expected<std::unique_ptr<Node>, std::string>;

}