/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/lights/light.hpp"
#include "vglx/math/utilities.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Represents a light that emits in a specific direction with a
 * cone-shaped area of influence.
 *
 * A spotlight combines directional and point light behavior: intensity
 * follows physical inverse-square falloff with distance and diminishes with
 * the angle from the central axis of the cone. This is commonly used to
 * simulate focused light sources such as flashlights or stage spotlights.
 *
 * When the @ref SpotLight::Parameters::target "target" parameter is set to
 * `nullptr`, the light will point toward the world origin.
 * The target must belong to the same @ref Scene as this object. Attaching
 * or referencing a target from a different scene results in undefined behavior
 * and is not supported.
 *
 * @code
 * my_scene->Add(vglx::SpotLight::Create({
 *   .color = 0xFFFFFFu,
 *   .intensity = 25.0f,
 *   .angle = vglx::math::DegToRad(10.0f),
 *   .penumbra = 0.3f,
 *   .target = nullptr,
 *   .range = 0.0f
 * }));
 * @endcode
 *
 * @ingroup LightsGroup
 */
class VGLX_EXPORT SpotLight : public Light {
public:
    /// @brief Parameters for constructing a @ref SpotLight object.
    struct Parameters {
        Color color {0xFFFFFFu}; ///< Light color.
        float intensity {1.0f}; ///< Light intensity multiplier.
        float angle {math::pi / 3.0f}; ///< Cone angle (in radians) for spotlight cutoff.
        float penumbra {0.0f}; ///< Softness of the spotlight edge.
        Node* target {nullptr}; ///< Node the light is directed toward.
        float range {0.0f}; ///< Maximum range of influence. 0 = unbounded.
        bool cast_shadow {false}; ///< Enables shadow casting for this light.
    };

    /// @brief Cone angle, in radians, of the spotlight.
    float angle;

    /// @brief Penumbra controlling the softness of the spotlight’s edge.
    float penumbra;

    /// @brief Node that the light is oriented toward.
    Node* target {nullptr};

    /**
     * @brief Maximum range of influence in world units.
     *
     * Falloff is smoothly windowed to reach zero at this distance.
     * A value of 0 disables the cutoff, leaving pure inverse-square falloff.
     */
    float range {0.0f};

    /// @brief When `true` this light casts shadows.
    bool cast_shadow {false};

    /// @brief Shadow configuration used when @ref cast_shadow is enabled.
    Shadow shadow {};

    /**
     * @brief Constructs a spotlight.
     *
     * @param params @ref SpotLight::Parameters "Initialization parameters"
     * for constructing the light.
     */
    explicit SpotLight(const Parameters& params);

    /**
     * @brief Creates an instance of @ref SpotLight with default parameters.
     */
    [[nodiscard]] static auto Create() -> std::unique_ptr<SpotLight> {
        return std::make_unique<SpotLight>(Parameters {});
    }

    /**
     * @brief Creates an instance of @ref SpotLight.
     *
     * @param params @ref SpotLight::Parameters "Initialization parameters"
     * for constructing the light.
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::unique_ptr<SpotLight> {
        return std::make_unique<SpotLight>(params);
    }

    /**
     * @brief Identifies this light as @ref Light::Type "Light::Type::Spot".
     */
    [[nodiscard]] auto GetType() const -> Light::Type override {
        return Light::Type::Spot;
    }

    /// @cond INTERNAL
    [[nodiscard]] auto GetShadow() -> Shadow* override;
    /// @endcond

    /**
     * @brief Returns the light's luminous power in lumens.
     *
     * Uses the convention $\Phi = \pi \cdot I$, where $I$ is
     * @ref Light::intensity "intensity" in candela, so power is independent
     * of the cone @ref angle.
     */
    [[nodiscard]] auto GetPower() const -> float {
        return intensity * math::pi;
    }

    /**
     * @brief Sets @ref Light::intensity "intensity" from luminous power
     * in lumens.
     *
     * @param power Luminous power in lumens.
     */
    auto SetPower(float power) -> void {
        intensity = power / math::pi;
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
