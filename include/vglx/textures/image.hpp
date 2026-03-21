/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include <cstdint>
#include <vector>

namespace vglx {

/**
 * @brief Represents decoded image data loaded from an asset.
 *
 * An image holds raw pixel data along with the metadata needed to
 * interpret it: dimensions and color space. Copying is disabled to
 * prevent accidental duplication of large pixel buffers. Use `std\::move`
 * to transfer ownership.
 *
 * @ingroup TexturesGroup
 */
struct VGLX_EXPORT Image {
    /**
     * @brief Enumerates supported color spaces for image data.
     *
     * Textures containing color data (e.g. albedo or base color maps) are
     * typically authored in sRGB and should use @ref ColorSpace "ColorSpace::sRGB".
     * Data textures (e.g. normals, roughness, metallic, masks) must use
     * @ref ColorSpace "ColorSpace::Linear" to preserve numerical correctness.
     */
    enum class ColorSpace {
        Linear, ///< Linear color space, no gamma correction is applied.
        sRGB ///< sRGB color space, converted to linear on sampling.
    };

    /**
     * @brief Constructs an image from pixel data.
     *
     * @param data Raw pixel bytes. Ownership is transferred via move.
     * @param width Image width in pixels.
     * @param height Image height in pixels.
     * @param color_space Color space of the pixel data.
     */
    Image(
        std::vector<uint8_t> data,
        unsigned width,
        unsigned height,
        ColorSpace color_space = ColorSpace::sRGB
    ) : data(std::move(data)), width(width), height(height), color_space(color_space) {}

    // Non-copyable
    Image(const Image&) = delete;
    auto operator=(const Image&) -> Image& = delete;

    Image(Image&&) noexcept = default;
    auto operator=(Image&&) noexcept -> Image&  = default;

    /// @brief Raw pixel data.
    std::vector<uint8_t> data;

    /// @brief Image width in pixels.
    unsigned width;

    /// @brief Image height in pixels.
    unsigned height;

    /// @brief Color space of the pixel data.
    ColorSpace color_space {ColorSpace::sRGB};
};

};