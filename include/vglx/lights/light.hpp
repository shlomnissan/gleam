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
 * @brief Per-light shadow configuration.
 *
 * Plain, user-tunable data owned by a shadow-casting light and exposed as a
 * public member. Holds only the settings that cannot be derived from the light.
 * The depth framebuffer, texture, and light-space projection are owned by the
 * renderer, and the projection is derived from the light's own properties.
 *
 * @ingroup LightsGroup
 */
struct Shadow {
    /**
     * @brief Constant depth bias applied to the shadow compare.
     *
     * Positive values push the compare toward the light, reducing
     * self-shadowing artifacts. Overly large values cause shadows
     * to visibly detach from their casters.
     */
    float bias {0.0f};

    /**
     * @brief Near plane of the shadow camera.
     */
    float near {0.5f};

    /**
     * @brief Far plane of the shadow camera.
     *
     * Directional lights always use this value. Point lights and spot lights with
     * a positive range use the range instead.
     *
     */
    float far {500.0f};

    /**
     * @brief Half-width of the directional shadow camera's orthographic box.
     */
    float extent {15.0f};

    /**
     * @brief Penumbra width in shadow map texels.
     *
     * Values in the range $[1, 2]$ stay smooth. Larger values trade quality for
     * width. Only used when @ref Renderer::ShadowMap "PCF" is selected.
     */
    float radius {1.0f};

    /**
     * @brief Resolution of the square shadow map.
     */
    unsigned int map_size {1024};

    /**
     * @brief Re-renders the shadow map every frame.
     *
     * Disable for lights whose casters are static to skip their shadow
     * passes. The light and its casters must not move while disabled.
     */
    bool auto_update {true};

    /**
     * @brief Requests a one-time shadow map refresh when @ref auto_update
     * is disabled. Cleared by the renderer after the next render.
     */
    bool needs_update {false};
};

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

    /// @brief Light color represented as @ref Color "RGB components".
    Color color {0xffffffu};

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

    /// @cond INTERNAL
    [[nodiscard]] virtual auto GetShadow() -> Shadow* {
        return nullptr;
    }
    /// @endcond

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
