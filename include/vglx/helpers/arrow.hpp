/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/math/color.hpp"
#include "vglx/math/vector3.hpp"
#include "vglx/scene/node.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Visual debug primitive representing a 3D directional arrow.
 *
 * Arrow is a lightweight scene node used for debugging spatial relationships:
 * normals, force vectors, light directions, coordinate axes, or any vector-based
 * visualization. The arrow is defined by an origin point, a direction, a length,
 * and a color. Its geometry is generated automatically and updates whenever the
 * direction or origin is changed.
 *
 * @code
 * my_scene->Add(vglx::Arrow::Create({
 *   .direction = {1.0f, 0.0f, 0.0f},
 *   .origin = Vector3::Zero(),
 *   .color = 0xFF0000,
 *   .length = 1.0f
 * }));
 * @endcode
 *
 * @ingroup HelpersGroup
 */
class VGLX_EXPORT Arrow : public Node {
public:
    /// @brief Parameters used to construct an @ref Arrow object.
    struct Parameters {
        Vector3 direction {0.0f, 0.0f, 1.0f}; ///< Direction vector of the arrow.
        Vector3 origin {Vector3::Zero()}; ///< World-space origin of the arrow.
        Color color {0xFFFFFF}; ///< Arrow color.
        float length {1.0f}; ///< Length of the arrow in world units.
    };

    /**
     * @brief Constructs an arrow node from the given parameters.
     *
     * @param params @ref Arrow::Parameters "Initialization parameters"
     * for constructing the arrow.
     */
    explicit Arrow(const Parameters& params);

    /**
     * @brief Creates an instance of @ref Arrow with default parameters.
     */
    [[nodiscard]] static auto Create() -> std::unique_ptr<Arrow> {
        return std::make_unique<Arrow>(Parameters {});
    }

    /**
     * @brief Creates an instance of @ref Arrow.
     *
     * @param params @ref Arrow::Parameters "Initialization parameters"
     * for constructing the arrow.
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::unique_ptr<Arrow> {
        return std::make_unique<Arrow>(params);
    }

    /**
     * @brief Updates the direction of the arrow.
     *
     * Automatically recalculates the internal transform so the arrow points
     * in the new direction.
     *
     * @param direction New direction vector.
     */
    auto SetDirection(const Vector3& direction) -> void;

    /**
     * @brief Updates the origin of the arrow.
     *
     * Moves the base of the arrow to the given world-space point.
     *
     * @param origin New origin position.
     */
    auto SetOrigin(const Vector3& origin) -> void;
};

}