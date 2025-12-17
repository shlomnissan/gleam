/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/lights/light.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Represents a light that emits evenly in all directions.
 *
 * An ambient light contributes a constant illumination to all objects in the
 * scene, regardless of their position or orientation. It does not have a
 * defined direction and therefore cannot cast shadows.
 *
 * Ambient lighting is typically used to simulate indirect or global
 * illumination and to prevent completely unlit regions in the scene.
 *
 * @code
 * auto ambient = vglx::AmbientLight::Create({
 *   .color = 0xFFFFFF,
 *   .intensity = 0.3f
 * });
 *
 * my_scene->Add(ambient);
 * @endcode
 *
 * @ingroup LightsGroup
 */
class VGLX_EXPORT AmbientLight : public Light {
public:
    /// @brief Parameters for constructing an @ref AmbientLight object.
    struct Parameters {
        Color color; ///< Light color.
        float intensity; ///< Light intensity multiplier.
    };

    /**
     * @brief Constructs an ambient light.
     *
     * @param params @ref AmbientLight::Parameters "Initialization parameters"
     * for constructing the light.
     */
    explicit AmbientLight(const Parameters& params) : Light(params.color, params.intensity) {
        SetName("ambient light");
    }

    /**
     * @brief Creates an instance of @ref AmbientLight.
     *
     * @param params @ref AmbientLight::Parameters "Initialization parameters"
     * for constructing the light.
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::unique_ptr<AmbientLight> {
        return std::make_unique<AmbientLight>(params);
    }

    /**
     * @brief Identifies this light as @ref Light::Type "Light::Type::Ambient".
     */
    [[nodiscard]] auto GetType() const -> Light::Type override {
        return Light::Type::Ambient;
    }
};

}