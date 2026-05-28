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
 * @brief Unlit material rendered without lighting.
 *
 * Renders surfaces using a uniform color or texture without applying any
 * illumination or shading. Useful for UI elements, overlays, debug geometry,
 * or stylized effects where lighting is not desired. Transparency is supported
 * through the alpha channel of the texture or a separate alpha map.
 *
 * @code
 * auto material = vglx::UnlitMaterial::Create({
 *   .color = 0xFFFFFF,
 *   .texture_map = texture
 * });
 *
 * my_scene->Add(vglx::Mesh::Create(geometry, material));
 * @endcode
 *
 * @ingroup MaterialsGroup
 */
class VGLX_EXPORT UnlitMaterial : public Material {
public:
    /**
     * @brief Parameters for constructing an @ref UnlitMaterial object.
     */
    struct Parameters {
        Color color = 0xFFFFFF; ///< Base surface color.
        std::shared_ptr<Texture> texture_map = nullptr; ///< Surface color texture.
        std::shared_ptr<Texture> alpha_map = nullptr; ///< Per-pixel opacity map.
    };

    /// @brief Base color applied to the surface.
    Color color;

    /// @brief Color texture sampled for rendering; alpha channel controls transparency.
    std::shared_ptr<Texture> texture_map;

    /// @brief Alpha-only texture providing per-pixel opacity.
    std::shared_ptr<Texture> alpha_map;

    /**
     * @brief Constructs an unlit material with default parameters.
     */
    UnlitMaterial() : UnlitMaterial(Parameters{}) {}

    /**
     * @brief Constructs an unlit material from the given parameters.
     *
     * @param params @ref UnlitMaterial::Parameters "Initialization parameters".
     */
    explicit UnlitMaterial(const Parameters& params)
      : color(params.color),
        texture_map(params.texture_map),
        alpha_map(params.alpha_map) {}

    /**
     * @brief Creates a shared instance of @ref UnlitMaterial with default parameters.
     */
    [[nodiscard]] static auto Create() -> std::shared_ptr<UnlitMaterial> {
        return std::make_shared<UnlitMaterial>();
    }

    /**
     * @brief Creates a shared instance of @ref UnlitMaterial.
     *
     * @param params @ref UnlitMaterial::Parameters "Initialization parameters".
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::shared_ptr<UnlitMaterial> {
        return std::make_shared<UnlitMaterial>(params);
    }

    /**
     * @brief Identifies this material as
     * @ref Material::Type "Material::Type::UnlitMaterial".
     */
    auto GetType() const -> Type override {
        return Material::Type::UnlitMaterial;
    }
};

}
