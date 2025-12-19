/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/math/color.hpp"
#include "vglx/scene/node.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Abstract base class for all light types.
 *
 * This class is not intended to be used directly. Use one of the concrete
 * light types such as @ref AmbientLight, @ref DirectionalLight, @ref PointLight, or
 * @ref SpotLight.
 *
 * @ingroup LightsGroup
 */
class VGLX_EXPORT Light : public Node {
public:
    /**
     * @brief Enumerates all supported light types.
     */
    enum class Type {
        Ambient, ///< Non-directional constant light.
        Directional, ///< Infinitely distant light with parallel rays.
        Point, ///< Omnidirectional light emitting from a single point.
        Spot ///< Light emitting in a cone from a single point.
    };

    /**
     * @brief Defines how light intensity diminishes over distance.
     *
     * Used by point and spot lights to simulate physical light falloff.
     * The attenuation model follows the classic convention:
     *
     * $$
     * I = \frac{1.0}{a + b \cdot d + c \cdot d^2}
     * $$
     *
     * where $a$, $b$, and $c$ correspond to base, linear, and quadratic
     * components respectively, and $d$ is the distance from the light source.
     */
    struct Attenuation {
        /// @brief Constant term unaffected by distance.
        float base {1.0f};
        /// @brief Linear fade with distance.
        float linear {0.0f};
        /// @brief Quadratic fade (inverse-square falloff).
        float quadratic {0.0f};
    };

    /// @brief Light color represented as @ref Color "RGB components".
    Color color {0xffffff};

    /// @brief Intensity multiplier applied to @ref Light::color "color".
    float intensity {1.0f};

    /**
     * @brief Constructs a new light.
     *
     * @param color Light color.
     * @param intensity Light intensity multiplier.
     */
    Light(Color color, float intensity) : color(color), intensity(intensity) {}

    /**
     * @brief Returns the specific @ref Type of this light.
     */
    [[nodiscard]] virtual auto GetType() const -> Light::Type = 0;

    /**
     * @brief Identifies this node as @ref Node::Type "Node::Type::Light".
     */
    [[nodiscard]] auto GetNodeType() const -> Node::Type override {
        return Node::Type::Light;
    }

    /**
     * @brief Enables or disables debug visualization mode.
     *
     * This mode can be used by renderers to draw light representations
     * such as frustums, spheres, or cones for debugging purposes.
     *
     * @param is_debug_mode `true` to enable debug mode; `false` to disable.
     */
    virtual auto SetDebugMode(bool is_debug_mode) -> void {
        debug_mode_enabled_ = is_debug_mode;
    }

    virtual ~Light() = default;

protected:
    bool debug_mode_enabled_ {false};
};

}