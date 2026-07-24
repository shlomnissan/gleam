/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/materials/material.hpp"
#include "vglx/textures/texture_2d.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Depth-only material used for shadow map rendering.
 *
 * Writes fragment depth without producing any color output. The renderer uses
 * a single instance of this material internally to draw shadow casters,
 * reconfiguring it per object, so it rarely needs to be created directly.
 *
 * The texture maps exist solely to feed the @ref Material::alpha_test
 * "alpha test". When a caster uses cutout transparency, its maps are assigned
 * here so that discarded fragments leave holes in the shadow map. Color
 * content is ignored; only the sampled alpha participates.
 *
 * @code
 * auto material = vglx::DepthMaterial::Create({
 *   .albedo_map = foliage_texture
 * });
 *
 * material->alpha_test = 0.5f;
 * @endcode
 *
 * @ingroup MaterialsGroup
 */
class VGLX_EXPORT DepthMaterial : public Material {
public:
    /**
     * @brief Parameters for constructing a @ref DepthMaterial object.
     */
    struct Parameters {
        std::shared_ptr<Texture> albedo_map {nullptr}; ///< Color texture whose alpha channel feeds the alpha test.
        std::shared_ptr<Texture> alpha_map {nullptr}; ///< Alpha-only texture feeding the alpha test.
    };

    /// @brief Color texture sampled for its alpha channel only.
    std::shared_ptr<Texture> albedo_map;

    /// @brief Alpha-only texture providing per-pixel opacity.
    std::shared_ptr<Texture> alpha_map;

    /**
     * @brief Constructs a depth material with default parameters.
     */
    DepthMaterial() : DepthMaterial(Parameters{}) {}

    /**
     * @brief Constructs a depth material from the given parameters.
     *
     * @param params @ref DepthMaterial::Parameters "Initialization parameters".
     */
    explicit DepthMaterial(const Parameters& params)
      : albedo_map(params.albedo_map),
        alpha_map(params.alpha_map) {}

    /**
     * @brief Creates a shared instance of @ref DepthMaterial with default parameters.
     */
    [[nodiscard]] static auto Create() -> std::shared_ptr<DepthMaterial> {
        return std::make_shared<DepthMaterial>();
    }

    /**
     * @brief Creates a shared instance of @ref DepthMaterial.
     *
     * @param params @ref DepthMaterial::Parameters "Initialization parameters".
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::shared_ptr<DepthMaterial> {
        return std::make_shared<DepthMaterial>(params);
    }

    /**
     * @brief Identifies this material as
     * @ref Material::Type "Material::Type::DepthMaterial".
     */
    auto GetType() const -> Type override {
        return Material::Type::DepthMaterial;
    }
};

}
