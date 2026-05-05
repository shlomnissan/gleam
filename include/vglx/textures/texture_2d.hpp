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
 * rendering. Textures are typically loaded using @ref LoadTexture rather
 * than instantiated directly.
 *
 * @code
 * MyScene::MyScene() {
 *   auto texture = vglx::LoadTexture("assets/diffuse.png");
 *
 *   if (texture.has_value()) {
 *     material_->texture_map = texture.value();
 *   } else {
 *     std::println(stderr, "{}", texture.error());
 *   }
 * }
 * @endcode
 *
 * To learn more about how textures are imported and loaded see the
 * [Importing Assets Guide](/manual/importing_assets).
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
     * @brief Returns the UV transformation matrix.
     *
     * The transform can be modified through translation, scaling, or rotation
     * using the provided helper methods. This affects how the texture is
     * sampled during rendering.
     */
    [[nodiscard]] auto GetTransform() -> Matrix3 {
        return transform_.Get();
    }

    /**
     * @brief Applies a translation offset along the X-axis.
     *
     * @param value Offset value in pixels.
     */
    auto OffsetX(float value) { transform_.Translate({value, 0.0f}); }

    /**
     * @brief Applies a translation offset along the Y-axis.
     *
     * @param value Offset value in pixels.
     */
    auto OffsetY(float value) { transform_.Translate({0.0f, value}); }

    /**
     * @brief Applies a uniform scale to the texture coordinates.
     *
     * @param value Scale factor.
     */
    auto Scale(float value) { transform_.Scale({value, value}); }

    /**
     * @brief Applies a rotation to the texture coordinates.
     *
     * @param angle Rotation angle in radians.
     */
    auto Rotate(float angle) { transform_.Rotate(angle); }

private:
    Transform2 transform_;
};

}