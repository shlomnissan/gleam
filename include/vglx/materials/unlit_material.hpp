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
 * auto material = vglx::UnlitMaterial::Create(0xFFFFFF);
 * material->texture_map = texture;
 *
 * my_scene->Add(vglx::Mesh::Create(geometry, material));
 * @endcode
 *
 * @ingroup MaterialsGroup
 */
class VGLX_EXPORT UnlitMaterial : public Material {
public:
    /// @brief Base color applied to the surface.
    Color color = 0xFFFFFF;

    /// @brief Color texture sampled for rendering; alpha channel controls transparency.
    std::shared_ptr<Texture> texture_map = nullptr;

    /// @brief Alpha-only texture providing per-pixel opacity.
    std::shared_ptr<Texture> alpha_map = nullptr;

    /**
     * @brief Constructs an unlit material.
     *
     * @param color Base color of the material.
     */
    explicit UnlitMaterial(const Color& color) : color(color) {}

    /**
     * @brief Creates a shared instance of @ref UnlitMaterial.
     *
     * @param color Base color of the material.
     */
    [[nodiscard]] static auto Create(const Color& color = 0xFFFFFF) -> std::shared_ptr<UnlitMaterial> {
        return std::make_shared<UnlitMaterial>(color);
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