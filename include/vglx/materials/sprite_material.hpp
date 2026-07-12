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
 * auto material = vglx::SpriteMaterial::Create({
 *   .color = 0xFFFFFF,
 *   .texture_map = texture
 * });
 *
 * my_scene->Add(vglx::Sprite::Create(material));
 * @endcode
 *
 * @ingroup MaterialsGroup
 */
class VGLX_EXPORT SpriteMaterial : public Material {
public:
    /**
     * @brief Parameters for constructing a @ref SpriteMaterial object.
     */
    struct Parameters {
        Color color {0xFFFFFF}; ///< Base tint color applied to the texture.
        std::shared_ptr<Texture> texture_map {nullptr}; ///< Sprite texture sampled for color and alpha.
        bool size_attenuation {true}; ///< Attenuate sprite size by distance in perspective.
    };

    /// @brief Base tint color applied multiplicatively to the sprite texture.
    Color color;

    /// @brief Sprite texture sampled in RGBA; alpha controls transparency.
    std::shared_ptr<Texture> texture_map;

    /// @brief Controls whether sprite size is attenuated by distance in perspective.
    bool size_attenuation {true};

    /**
     * @brief Constructs a sprite material from the given parameters.
     *
     * Transparency is enabled by default and uses the alpha channel of the
     * provided texture.
     *
     * @param params @ref SpriteMaterial::Parameters "Initialization parameters".
     */
    explicit SpriteMaterial(const Parameters& params)
      : color(params.color),
        texture_map(params.texture_map),
        size_attenuation(params.size_attenuation) {
        transparent = true;
    }

    /**
     * @brief Creates a shared instance of @ref SpriteMaterial with default parameters.
     */
    [[nodiscard]] static auto Create() -> std::shared_ptr<SpriteMaterial> {
        return std::make_shared<SpriteMaterial>(Parameters {});
    }

    /**
     * @brief Creates a shared instance of @ref SpriteMaterial.
     *
     * @param params @ref SpriteMaterial::Parameters "Initialization parameters".
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::shared_ptr<SpriteMaterial> {
        return std::make_shared<SpriteMaterial>(params);
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