/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace vglx {

/**
 * @brief Represents decoded image data loaded from an asset.
 *
 * An image holds raw pixel data along with its dimensions. Images are
 * typically created through @ref Image::Create and shared between one
 * or more @ref Texture2D instances via `std::shared_ptr`.
 *
 * @ingroup TexturesGroup
 */
struct VGLX_EXPORT Image {
    /**
     * @brief Parameters for constructing an @ref Image object.
     */
    struct Parameters {
        std::vector<uint8_t> data {}; ///< Raw pixel bytes.
        unsigned width {0}; ///< Image width in pixels.
        unsigned height {0}; ///< Image height in pixels.
    };

    /**
     * @brief Constructs an image from initialization parameters.
     *
     * Prefer @ref Image::Create over direct construction to obtain a
     * `std::shared_ptr<Image>` that can be shared between textures.
     *
     * @param params @ref Image::Parameters "Initialization parameters"
     * for constructing the image.
     */
    Image(Parameters params)
        : data(std::move(params.data)),
          width(params.width),
          height(params.height) {}

    // Non-copyable
    Image(const Image&) = delete;
    auto operator=(const Image&) -> Image& = delete;

    // Moveable
    Image(Image&&) noexcept = default;
    auto operator=(Image&&) noexcept -> Image&  = default;

    /**
     * @brief Creates a shared instance of @ref Image.
     *
     * @param params @ref Image::Parameters "Initialization parameters"
     * for constructing the image.
     */
    static auto Create(Parameters params) -> std::shared_ptr<Image> {
        return std::make_shared<Image>(std::move(params));
    }

    /// @brief Raw pixel data.
    std::vector<uint8_t> data;

    /// @brief Image width in pixels.
    unsigned width;

    /// @brief Image height in pixels.
    unsigned height;
};

};