/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/materials/material.hpp"
#include "vglx/math/color.hpp"
#include "vglx/textures/texture_2d.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Lightweight material for textured 2D sprites and billboards.
 *
 * Designed for camera-facing quads such as UI elements, particles, and glyphs.
 * Renders without lighting (unlit) and uses an RGBA texture with an optional
 * tint color. Transparency is enabled by default and driven by the texture's
 * alpha channel.
 *
 * @code
 * auto material = vglx::SpriteMaterial::Create(texture, 0xFFFFFF);
 * auto sprite = vglx::Sprite::Create(material);
 *
 * my_scene->Add(sprite);
 * @endcode
 *
 * @ingroup MaterialsGroup
 */
class VGLX_EXPORT SpriteMaterial : public Material {
public:
    /// @brief Base tint color applied multiplicatively to the sprite texture.
    Color color;

    /// @brief Sprite texture sampled in RGBA; alpha controls transparency.
    std::shared_ptr<Texture2D> texture_map;

    /**
     * @brief Constructs a sprite material.
     *
     * Transparency is enabled by default and uses the alpha channel of the
     * provided texture.
     *
     * @param texture_map Sprite texture to sample for color and alpha.
     * @param color Base tint color applied to the texture.
     */
    SpriteMaterial(
        std::shared_ptr<Texture2D> texture_map,
        const Color& color = 0xFFFFFF
    ) : color(color), texture_map(std::move(texture_map)) {
        transparent = true;
    }

    /**
     * @brief Creates a shared instance of @ref SpriteMaterial.
     *
     * @param texture_map Sprite texture to sample for color and alpha.
     * @param color Base tint color applied to the texture.
     */
    [[nodiscard]] static auto Create(
        std::shared_ptr<Texture2D> texture_map,
        const Color& color = 0xFFFFFF
    ) -> std::shared_ptr<SpriteMaterial> {
        return std::make_shared<SpriteMaterial>(std::move(texture_map), color);
    }

    /**
     * @brief Identifies this material as
     * @ref Material::Type "Material::Type::SpriteMaterial".
     */
    auto GetType() const -> Type override {
        return Material::Type::SpriteMaterial;
    }
};

}