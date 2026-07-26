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
 * @brief Represents a light that emits from a single point in all directions.
 *
 * A point light simulates a localized light source such as a bare lightbulb.
 * Intensity follows physical inverse-square falloff, so @ref Light::intensity
 * "intensity" represents the light's brightness at a distance of one unit.
 *
 * @code
 * my_scene->Add(vglx::PointLight::Create({
 *   .color = 0xFFFFFF,
 *   .intensity = 25.0f,
 *   .range = 0.0f
 * }));
 * @endcode
 *
 * @ingroup LightsGroup
 */
class VGLX_EXPORT PointLight : public Light {
public:
    /// @brief Parameters for constructing a @ref PointLight object.
    struct Parameters {
        Color color {0xFFFFFF}; ///< Light color.
        float intensity {1.0f}; ///< Light intensity multiplier.
        float range {0.0f}; ///< Maximum range of influence. 0 = unbounded.
        bool cast_shadow {false}; ///< Enables shadow casting for this light.
    };

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
     * @brief Constructs a point light.
     *
     * @param params @ref PointLight::Parameters "Initialization parameters"
     * for constructing the light.
     */
    explicit PointLight(const Parameters& params);

    /**
     * @brief Creates an instance of @ref PointLight with default parameters.
     */
    [[nodiscard]] static auto Create() -> std::unique_ptr<PointLight> {
        return std::make_unique<PointLight>(Parameters {});
    }

    /**
     * @brief Creates an instance of @ref PointLight.
     *
     * @param params @ref PointLight::Parameters "Initialization parameters"
     * for constructing the light.
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::unique_ptr<PointLight> {
        return std::make_unique<PointLight>(params);
    }

    /**
     * @brief Identifies this light as @ref Light::Type "Light::Type::Point".
     */
    [[nodiscard]] auto GetType() const -> Light::Type override {
        return Light::Type::Point;
    }

    /// @cond INTERNAL
    [[nodiscard]] auto GetShadow() -> Shadow* override;
    /// @endcond

    /**
     * @brief Returns the light's luminous power in lumens.
     *
     * Power is the total light emitted in all directions:
     * $\Phi = 4\pi \cdot I$, where $I$ is @ref Light::intensity "intensity"
     * in candela.
     */
    [[nodiscard]] auto GetPower() const -> float {
        return intensity * 4.0f * math::pi;
    }

    /**
     * @brief Sets @ref Light::intensity "intensity" from luminous power
     * in lumens.
     *
     * Useful for specifying brightness in familiar lightbulb terms,
     * e.g. `SetPower(800.0f)` approximates a 60W incandescent bulb.
     *
     * @param power Luminous power in lumens.
     */
    auto SetPower(float power) -> void {
        intensity = power / (4.0f * math::pi);
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
