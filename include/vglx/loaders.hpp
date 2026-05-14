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

/**
 * @defgroup LoadersGroup Loaders
 * @brief Free functions for loading external resources.
 */

namespace vglx {

namespace fs = std::filesystem;

struct Image;
class Texture2D;
class CubeTexture;
class Node;

/**
 * @brief Six face image paths for a cube texture.
 *
 * @ingroup LoadersGroup
 */
struct CubeTexturePaths {
    fs::path positive_x; ///< Right face (+X).
    fs::path negative_x; ///< Left face (-X).
    fs::path positive_y; ///< Top face (+Y).
    fs::path negative_y; ///< Bottom face (-Y).
    fs::path positive_z; ///< Front face (+Z).
    fs::path negative_z; ///< Back face (-Z).
};

/**
 * @brief Loads raw image data from disk.
 *
 * @code
 * auto image = vglx::LoadImage("assets/heightmap.png");
 * if (image.has_value()) {
 *     // use image.value()
 * } else {
 *     std::println(stderr, "{}", image.error());
 * }
 * @endcode
 *
 * @param path Filesystem path to the image asset.
 *
 * @ingroup LoadersGroup
 */
[[nodiscard]] VGLX_EXPORT auto LoadImage(
    const fs::path& path
) -> std::expected<std::shared_ptr<Image>, std::string>;

/**
 * @brief Loads a 2D texture from disk.
 *
 * @code
 * auto texture = vglx::LoadTexture("assets/crate.png");
 * if (texture.has_value()) {
 *     // use texture.value()
 * } else {
 *     std::println(stderr, "{}", texture.error());
 * }
 * @endcode
 *
 * @param path Filesystem path to the texture asset.
 * @param color_space Color space the texture is interpreted in.
 *
 * @ingroup LoadersGroup
 */
[[nodiscard]] VGLX_EXPORT auto LoadTexture(
    const fs::path& path,
    Texture::ColorSpace color_space = Texture::ColorSpace::sRGB
) -> std::expected<std::shared_ptr<Texture2D>, std::string>;

/**
 * @brief Loads a cube texture from six face images on disk.
 *
 * @code
 * auto skybox = vglx::LoadCubeTexture({
 *     .positive_x = "assets/skybox/px.jpg",
 *     .negative_x = "assets/skybox/nx.jpg",
 *     .positive_y = "assets/skybox/py.jpg",
 *     .negative_y = "assets/skybox/ny.jpg",
 *     .positive_z = "assets/skybox/pz.jpg",
 *     .negative_z = "assets/skybox/nz.jpg",
 * });
 *
 * if (skybox.has_value()) {
 *     // use skybox.value()
 * } else {
 *     std::println(stderr, "{}", skybox.error());
 * }
 * @endcode
 *
 * @param paths Filesystem paths to the six face images.
 * @param color_space Color space the cube texture is interpreted in.
 *
 * @ingroup LoadersGroup
 */
[[nodiscard]] VGLX_EXPORT auto LoadCubeTexture(
    const CubeTexturePaths& paths,
    Texture::ColorSpace color_space = Texture::ColorSpace::sRGB
) -> std::expected<std::shared_ptr<CubeTexture>, std::string>;

/**
 * @brief Loads a mesh asset from disk.
 *
 * @code
 * auto root = vglx::LoadMesh("assets/robot/robot.obj");
 * if (root.has_value()) {
 *     // use root.value()
 * } else {
 *     std::println(stderr, "{}", root.error());
 * }
 * @endcode
 *
 * @param path Filesystem path to the mesh asset.
 *
 * @ingroup LoadersGroup
 */
[[nodiscard]] VGLX_EXPORT auto LoadMesh(
    const fs::path& path
) -> std::expected<std::unique_ptr<Node>, std::string>;

}
