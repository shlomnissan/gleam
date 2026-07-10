/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/lights/light.hpp"
#include "vglx/math/vector3.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Represents a light that emits in a single direction.
 *
 * A directional light simulates an infinitely distant light source, such as
 * sunlight. All light rays are considered parallel, and the light has no
 * position in space, only direction. This makes it ideal for large-scale
 * illumination like outdoor or scene-wide lighting.
 *
 * When the @ref DirectionalLight::Parameters "target" parameter is
 * set to `nullptr`, the light will point toward the world origin.
 * The target must belong to the same @ref Scene as this object. Attaching
 * or referencing a target from a different scene results in undefined behavior
 * and is not supported.
 *
 * @code
 * my_scene->Add(vglx::DirectionalLight::Create({
 *   .color = 0xFFFFFF,
 *   .intensity = 1.0f,
 *   .target = nullptr
 * }));
 * @endcode
 *
 * @ingroup LightsGroup
 */
class VGLX_EXPORT DirectionalLight : public Light {
public:
    /// @brief Parameters for constructing a @ref DirectionalLight object.
    struct Parameters {
        Color color; ///< Light color.
        float intensity; ///< Light intensity multiplier.
        Node* target; ///< Node the light is directed toward.
        bool cast_shadow {false}; ///< Enables shadow casting for this light.
    };

    /// @brief Node that the light is oriented toward.
    Node* target {nullptr};

    /// @brief When `true` this light casts shadows.
    bool cast_shadow {false};

    /// @brief Shadow configuration used when @ref cast_shadow is enabled.
    Shadow shadow {};

    /**
     * @brief Constructs a directional light.
     *
     * @param params @ref DirectionalLight::Parameters "Initialization parameters"
     * for constructing the light.
     */
    explicit DirectionalLight(const Parameters& params);

    /**
     * @brief Creates an instance of @ref DirectionalLight.
     *
     * @param params @ref DirectionalLight::Parameters "Initialization parameters"
     * for constructing the light.
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::unique_ptr<DirectionalLight> {
        return std::make_unique<DirectionalLight>(params);
    }

    /**
     * @brief Identifies this light as @ref Light::Type "Light::Type::DirectionalLight".
     */
    auto GetType() const -> Light::Type override {
        return Light::Type::Directional;
    }

    /// @cond INTERNAL
    [[nodiscard]] auto GetShadow() const -> const Shadow* override;
    /// @endcond

    /**
     * @brief Returns the normalized direction vector of the light.
     *
     * The direction is derived from the light’s position and its
     * @ref DirectionalLight::target "target" node. If no target is set,
     * the light will point toward the origin.
     */
    [[nodiscard]] auto Direction() -> Vector3;

    /**
     * @brief Enables or disables debug visualization for this light.
     *
     * When enabled, the renderer will visualize the light’s direction
     * using a line and a unit plane facing the target.
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

    ~DirectionalLight() override;

private:
    /// @cond INTERNAL
    struct Impl;
    std::unique_ptr<Impl> impl_;
    /// @endcond
};

}
