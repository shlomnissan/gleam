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
 * @brief Represents a light that emits in a specific direction with a
 * cone-shaped area of influence.
 *
 * A spotlight combines directional and point light behavior: intensity
 * diminishes with distance (via attenuation) and with the angle from the
 * central axis of the cone. This is commonly used to simulate focused light
 * sources such as flashlights or stage spotlights.
 *
 * When the @ref SpotLight::Parameters::target "target" parameter is set to
 * `nullptr`, the light will point toward the world origin.
 *
 * @code
 * auto spot = vglx::SpotLight::Create({
 *   .color = 0xFFFFFF,
 *   .intensity = 1.0f,
 *   .angle = vglx::math::DegToRad(10.0f),
 *   .penumbra = 0.3f,
 *   .target = nullptr,
 *   .attenuation = {
 *     .base = 1.0f,
 *     .linear = 0.0f,
 *     .quadratic = 0.0f
 *   }
 * });
 * @endcode
 *
 * @ingroup LightsGroup
 */
class VGLX_EXPORT SpotLight : public Light {
public:
    /// @brief Parameters for constructing a @ref SpotLight object.
    struct Parameters {
        Color color; ///< Light color.
        float intensity; ///< Light intensity multiplier.
        float angle; ///< Cone angle (in radians) for spotlight cutoff.
        float penumbra; ///< Softness of the spotlight edge.
        std::shared_ptr<Node> target; ///< Node the light is directed toward.
        Attenuation attenuation; ///< Attenuation parameters controlling distance-based falloff.
    };

    /// @brief Cone angle, in radians, of the spotlight.
    float angle;

    /// @brief Penumbra controlling the softness of the spotlight’s edge.
    float penumbra;

    /// @brief Node that the light is oriented toward.
    std::shared_ptr<Node> target {nullptr};

    /// @brief Attenuation parameters controlling distance-based falloff.
    Attenuation attenuation;

    /**
     * @brief Constructs a spotlight.
     *
     * @param params @ref SpotLight::Parameters "Initialization parameters"
     * for constructing the light.
     */
    explicit SpotLight(const Parameters& params);

    /**
     * @brief Creates a shared instance of @ref SpotLight.
     *
     * @param params @ref SpotLight::Parameters "Initialization parameters"
     * for constructing the light.
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::shared_ptr<SpotLight> {
        return std::make_shared<SpotLight>(params);
    }

    /**
     * @brief Identifies this light as @ref Light::Type "Light::Type::Spot".
     */
    [[nodiscard]] auto GetType() const -> Light::Type override {
        return Light::Type::Spot;
    }

    /**
     * @brief Returns the normalized direction vector of the light.
     *
     * The direction is derived from the light’s position and its
     * @ref SpotLight::target "target" node. If no target is set,
     * the light will point toward the origin.
     */
    [[nodiscard]] auto Direction() -> Vector3;

    /**
     * @brief Enables or disables debug visualization for this light.
     *
     * When enabled, the renderer will visualize the spotlight cone and
     * influence region using helper line geometry.
     *
     * @param is_debug_mode `true` to enable debug mode; `false` to disable.
     */
    auto SetDebugMode(bool is_debug_mode) -> void override;

    /**
     * @brief Called once per frame to update the light state.
     *
     * Currently used to generate or dispose of debug geometry when
     * debug mode is enabled.
     *
     * @param delta Time in seconds since the last frame.
     */
    auto OnUpdate(float delta) -> void override;

    ~SpotLight() override;

private:
    /// @cond INTERNAL
    struct Impl;
    std::unique_ptr<Impl> impl_;
    /// @endcond
};

}