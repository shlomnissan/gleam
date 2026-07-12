/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <cstdint>
#include <memory>
#include <variant>
#include <vector>

namespace vglx {

/**
 * @brief Represents decoded image data loaded from an asset.
 *
 * An image holds raw pixel data along with its dimensions. Images are
 * typically loaded using an image loader and shared between
 * texture instances. Pixel data is stored as either 8-bit bytes for
 * standard LDR images, or 32-bit floats for HDR images. To learn more
 * see the [Importing Assets Guide](/manual/importing_assets).
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
 * Loading images directly is useful when you need to share the same
 * pixel data across textures or update texture contents at runtime
 * (see @ref DynamicTexture2D). For one-off texture creation, prefer
 * @ref LoadTexture, which loads an image and creates a texture in one step.
 *
 * @ingroup TexturesGroup
 */
struct Image {
    /// @brief Pixel data storage; LDR (8-bit) or HDR (32-bit float).
    using PixelData = std::variant<std::vector<uint8_t>, std::vector<float>>;

    /**
     * @brief Parameters for constructing an @ref Image object.
     */
    struct Parameters {
        PixelData data {}; ///< Raw pixel data.
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
    explicit Image(Parameters params)
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
    [[nodiscard]] static auto Create(Parameters params) -> std::shared_ptr<Image> {
        return std::make_shared<Image>(std::move(params));
    }

    /// @brief Raw pixel data.
    PixelData data;

    /// @brief Image width in pixels.
    unsigned width;

    /// @brief Image height in pixels.
    unsigned height;
};

};