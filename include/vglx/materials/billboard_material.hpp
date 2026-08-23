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
 * @brief Lightweight material for textured camera-facing billboards.
 *
 * Designed for camera-facing quads such as UI elements, particles, and glyphs.
 * Renders without lighting (unlit) and uses an RGBA texture with an optional
 * tint color. Transparency is enabled by default and driven by the texture's
 * alpha channel.
 *
 * @code
 * auto material = vglx::BillboardMaterial::Create({
 *   .color = 0xFFFFFFu,
 *   .texture_map = texture
 * });
 *
 * my_scene->Add(vglx::Billboard::Create(material));
 * @endcode
 *
 * @ingroup MaterialsGroup
 */
class VGLX_EXPORT BillboardMaterial : public Material {
public:
    /**
     * @brief Parameters for constructing a @ref BillboardMaterial object.
     */
    struct Parameters {
        Color color {0xFFFFFFu}; ///< Base tint color applied to the texture.
        std::shared_ptr<Texture> texture_map {nullptr}; ///< Billboard texture sampled for color and alpha.
        bool size_attenuation {true}; ///< Attenuate billboard size by distance in perspective.
    };

    /// @brief Base tint color applied multiplicatively to the billboard texture.
    Color color;

    /// @brief Billboard texture sampled in RGBA; alpha controls transparency.
    std::shared_ptr<Texture> texture_map;

    /// @brief Controls whether billboard size is attenuated by distance in perspective.
    bool size_attenuation {true};

    /**
     * @brief Constructs a billboard material from the given parameters.
     *
     * Transparency is enabled by default and uses the alpha channel of the
     * provided texture.
     *
     * @param params @ref BillboardMaterial::Parameters "Initialization parameters".
     */
    explicit BillboardMaterial(const Parameters& params)
      : color(params.color),
        texture_map(params.texture_map),
        size_attenuation(params.size_attenuation) {
        transparent = true;
    }

    /**
     * @brief Creates a shared instance of @ref BillboardMaterial with default parameters.
     */
    [[nodiscard]] static auto Create() -> std::shared_ptr<BillboardMaterial> {
        return std::make_shared<BillboardMaterial>(Parameters {});
    }

    /**
     * @brief Creates a shared instance of @ref BillboardMaterial.
     *
     * @param params @ref BillboardMaterial::Parameters "Initialization parameters".
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::shared_ptr<BillboardMaterial> {
        return std::make_shared<BillboardMaterial>(params);
    }

    /**
     * @brief Identifies this material as
     * @ref Material::Type "Material::Type::BillboardMaterial".
     */
    auto GetType() const -> Type override {
        return Material::Type::BillboardMaterial;
    }
};

}
