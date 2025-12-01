/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/math/transform2.hpp"
#include "vglx/textures/texture.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Represents a two-dimensional texture.
 *
 * A 2D texture stores image data that can be sampled by materials during
 * rendering. Textures are typically created through the engine’s
 * @ref TextureLoader "texture loader" rather than instantiated directly.
 *
 * @code
 * auto MyNode::OnAttached(SharedContextPointer context) -> void {
 *   context->texture_loader->LoadAsync(
 *     "assets/my_texture.tex",
 *     [this](auto result) {
 *       if (result) {
 *         texture_ = result.value();
 *       } else {
 *         std::println(stderr, "{}", result.error());
 *       }
 *     }
 *   );
 * }
 * @endcode
 *
 * @ingroup TexturesGroup
 */
class VGLX_EXPORT Texture2D : public Texture {
public:
    /// @brief Texture width in pixels.
    unsigned width;

    /// @brief Texture height in pixels.
    unsigned height;

    /// @brief Raw texture pixel data.
    std::vector<uint8_t> data;

    /**
     * @brief Parameters for constructing a @ref Texture2D object.
     */
    struct Parameters {
        unsigned width; ///< Width in pixels.
        unsigned height; ///< Height in pixels.
        std::vector<uint8_t> data; ///< Raw texture pixel data.
    };

    /**
     * @brief Constructs a 2D texture.
     *
     * @param params @ref Texture2D::Parameters "Initialization parameters"
     * for constructing the texture.
     */
    explicit Texture2D(const Parameters& params) :
        width(params.width),
        height(params.height),
        data(std::move(params.data)) {}

    /**
     * @brief Creates a shared instance of @ref Texture2D.
     *
     * @param params @ref Texture2D::Parameters "Initialization parameters"
     * for constructing the texture.
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::shared_ptr<Texture2D> {
        return std::make_shared<Texture2D>(params);
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