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
 * my_scene->fog = Fog::CreateLinear({
 *   .color = 0x444444u,
 *   .near = 2.0f,
 *   .far = 6.0f
 * });
 *
 * // Adds exponential fog to scene
 * my_scene->fog = Fog::CreateExponential({
 *   .color = 0x444444u,
 *   .density = 0.3f
 * });
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

    /**
     * @brief Parameters for constructing a linear @ref Fog object.
     */
    struct LinearParameters {
        Color color {0xFFFFFFu}; ///< Fog color.
        float near {1.0f}; ///< Distance at which fog begins to appear.
        float far {1000.0f}; ///< Distance at which fog reaches full intensity.
    };

    /**
     * @brief Parameters for constructing an exponential @ref Fog object.
     */
    struct ExponentialParameters {
        Color color {0xFFFFFFu}; ///< Fog color.
        float density {0.00025f}; ///< Exponential density factor (higher values produce thicker fog).
    };

    Type type; ///< Fog attenuation model.
    Color color; ///< Fog color applied at full intensity.

    float near {1.0f}; ///< Start distance for linear fog (used when type is Linear).
    float far {1000.0f}; ///< End distance for linear fog (used when type is Linear).
    float density {0.00025f}; ///< Density factor for exponential fog (used when type is Exponential).

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
     * @param params @ref Fog::LinearParameters "Initialization parameters".
     */
    [[nodiscard]] static auto CreateLinear(const LinearParameters& params) -> Fog {
        auto out = Fog {Type::Linear, params.color};
        out.near = params.near;
        out.far = params.far;
        return out;
    }

    /**
     * @brief Creates an exponential fog object.
     *
     * Exponential fog increases with distance using a density parameter,
     * typically with a factor like $f(d) = e^{-\\mathrm{density} \\cdot d}$ or
     * a related variant, which produces a smooth atmospheric falloff.
     *
     * @param params @ref Fog::ExponentialParameters "Initialization parameters".
     */
    [[nodiscard]] static auto CreateExponential(const ExponentialParameters& params) -> Fog {
        auto out = Fog {Type::Exponential, params.color};
        out.density = params.density;
        return out;
    }

    /**
     * @brief Returns the fog type.
     */
    [[nodiscard]] auto GetType() const -> Type { return type; }
};

}
