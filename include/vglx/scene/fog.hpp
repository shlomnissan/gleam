/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/math/color.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Describes atmospheric fog applied to a scene.
 *
 * Fog is a scene-level effect that blends fragment color toward a fog color
 * based on distance from the camera. It can be used to suggest large scale,
 * hide distant detail, and soften transitions into the background.
 *
 * @code
 * // Adds linear fog to scene
 * my_scene->fog = Fog::CreateLinear(0x444444, 2.0f, 6.0f);
 *
 * // Adds exponential fog to scene
 * my_scene->fog = Fog::CreateExponential(0x444444, 0.3f);
 * @endcode
 *
 * @ingroup SceneGroup
 */
struct Fog {
    /**
     * @brief Fog attenuation models.
     *
     * Selects how fog intensity increases with distance. Linear fog uses a
     * depth range, while exponential fog uses a density-based curve that grows
     * smoothly with distance.
     */
    enum class Type {
        Linear, ///< Depth-based fog using a near and far distance range.
        Exponential, ///< Density-based fog using a continuous falloff curve.
    };

    Type type; ///< Fog attenuation model.
    Color color; ///< Fog color applied at full intensity.

    float near; ///< Start distance for linear fog (used when type is Linear).
    float far; ///< End distance for linear fog (used when type is Linear).
    float density; ///< Density factor for exponential fog (used when type is Exponential).

    /**
     * @brief Constructs a fog object.
     *
     * @param type Fog::Type specifying the attenuation model.
     * @param color Fog color used for fully fogged fragments.
     */
    explicit Fog(Type type, const Color& color) : type(type), color(color) {}

    /**
     * @brief Creates a linear fog object.
     *
     * Linear fog increases its effect given a distance range along the view
     * direction, typically using a factor similar to
     * $f(d) = \\mathrm{clamp}((d - near) / (far - near), 0, 1)$.
     *
     * @param color Fog color.
     * @param near Distance at which fog begins to appear.
     * @param far Distance at which fog reaches full intensity.
     */
    [[nodiscard]] static auto CreateLinear(const Color& color, float near, float far) -> std::unique_ptr<Fog> {
        auto out = std::make_unique<Fog>(Type::Linear, color);
        out->near = near;
        out->far = far;
        return out;
    }

    /**
     * @brief Creates an exponential fog object.
     *
     * Exponential fog increases with distance using a density parameter,
     * typically with a factor like $f(d) = e^{-\\mathrm{density} \\cdot d}$ or
     * a related variant, which produces a smooth atmospheric falloff.
     *
     * @param color Fog color.
     * @param density Exponential density factor (higher values produce thicker fog).
     */
    [[nodiscard]] static auto CreateExponential(const Color& color, float density) -> std::unique_ptr<Fog> {
        auto out = std::make_unique<Fog>(Type::Exponential, color);
        out->density = density;
        return out;
    }

    /**
     * @brief Returns the fog type.
     */
    [[nodiscard]] auto GetType() const -> Type { return type; }
};

}