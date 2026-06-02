/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/math/transform2.hpp"
#include "vglx/textures/image.hpp"
#include "vglx/textures/texture.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Represents a two-dimensional texture.
 *
 * A 2D texture stores image data that can be sampled by materials during
 * rendering. Textures are typically loaded using a texture loader rather
 * than instantiated directly. To learn more see the
 * [Importing Assets Guide](/manual/importing_assets).
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
 * @ingroup TexturesGroup
 */
class VGLX_EXPORT Texture2D : public Texture {
public:
    /**
     * @brief Constructs a 2D texture from an @ref Image.
     *
     * The texture holds a shared reference to the image, allowing multiple
     * textures to share the same underlying pixel data.
     *
     * @param image Decoded image containing pixel data and dimensions.
     */
    explicit Texture2D(std::shared_ptr<Image> image) : image(std::move(image)) {}

    /// @brief The source image backing this texture.
    std::shared_ptr<Image> image {nullptr};

    /**
     * @brief Creates a shared instance of @ref Texture2D.
     *
     * @param image Decoded image containing pixel data and dimensions.
     */
    [[nodiscard]] static auto Create(std::shared_ptr<Image> image) -> std::shared_ptr<Texture2D> {
        return std::make_shared<Texture2D>(std::move(image));
    }

    /**
     * @brief Identifies this texture as @ref Texture::Type "Texture::Type::Texture2D".
     */
    [[nodiscard]] auto GetType() const -> Texture::Type override {
        return Texture::Type::Texture2D;
    }

    /**
     * @brief UV transform applied to texture coordinates when sampling.
     *
     * Modify it directly through the @ref Transform2 interface (for example
     * @ref Transform2::SetScale, @ref Transform2::SetPosition, or
     * @ref Transform2::SetRotation) to translate, scale, or rotate the
     * texture. The renderer reads the resulting matrix each frame.
     */
    Transform2 transform;
};

}