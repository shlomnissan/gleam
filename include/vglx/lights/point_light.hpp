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
 * @brief Represents a light that emits from a single point in all directions.
 *
 * A point light simulates a localized light source such as a bare lightbulb.
 * Intensity falls off with distance according to the configured attenuation
 * parameters.
 *
 * @code
 * auto point = vglx::PointLight::Create({
 *   .color = 0xFFFFFF,
 *   .intensity = 1.0f,
 *   .attenuation = {
 *     .base = 1.0f,
 *     .linear = 0.0f,
 *     .quadratic = 0.0f
 *   }
 * });
 *
 * my_scene->Add(point);
 * @endcode
 *
 * @ingroup LightsGroup
 */
class VGLX_EXPORT PointLight : public Light {
public:
    /// @brief Parameters for constructing a @ref PointLight object.
    struct Parameters {
        Color color; ///< Light color.
        float intensity; ///< Light intensity multiplier.
        Attenuation attenuation; ///< Light attenuation parameters.
    };

    /// @brief Attenuation parameters controlling distance-based falloff.
    Attenuation attenuation;

    /**
     * @brief Constructs a point light.
     *
     * @param params @ref PointLight::Parameters "Initialization parameters"
     * for constructing the light.
     */
    PointLight(const Parameters& params);

    /**
     * @brief Creates a shared instance of @ref PointLight.
     *
     * @param params @ref PointLight::Parameters "Initialization parameters"
     * for constructing the light.
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::shared_ptr<PointLight> {
        return std::make_shared<PointLight>(params);
    }

    /**
     * @brief Identifies this light as @ref Light::Type "Light::Type::Point".
     */
    [[nodiscard]] auto GetType() const -> Light::Type override {
        return Light::Type::Point;
    }

    /**
     * @brief Enables or disables debug visualization for this light.
     *
     * When enabled, the renderer will visualize the light’s location
     * using a spherical line geometry.
     *
     * @param is_debug_mode `true` to enable debug mode; `false` to disable.
     */
    auto SetDebugMode(bool is_debug_mode) -> void override;

    /**
     * @brief Called once per frame to update the light state.
     *
     * Currently used to update the debug geometry when debug mode
     * is enabled.
     *
     * @param delta Time in seconds since the last frame.
     */
    auto OnUpdate(float delta) -> void override;

    ~PointLight() override;

private:
    /// @cond INTERNAL
    struct Impl;
    std::unique_ptr<Impl> impl_;
    /// @endcond
};

}