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
 * auto arrow = vglx::Arrow::Create({
 *   .direction = {1.0f, 0.0f, 0.0f},
 *   .origin = Vector3::Zero(),
 *   .color = 0xFF0000,
 *   .length = 1.0f
 * });
 *
 * my_scene->Add(arrow);
 * @endcode
 *
 * @ingroup HelpersGroup
 */
class VGLX_EXPORT Arrow : public Node {
public:
    /// @brief Parameters used to construct an @ref Arrow object.
    struct Parameters {
        Vector3 direction; ///< Direction vector of the arrow.
        Vector3 origin; ///< World-space origin of the arrow.
        Color color; ///< Arrow color.
        float length; ///< Length of the arrow in world units.
    };

    /**
     * @brief Constructs an arrow node from the given parameters.
     *
     * @param params @ref Arrow::Parameters "Initialization parameters"
     * for constructing the arrow.
     */
    explicit Arrow(const Parameters& params);

    /**
     * @brief Creates a shared instance of @ref Arrow.
     *
     * @param params @ref Arrow::Parameters "Initialization parameters"
     * for constructing the arrow.
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::shared_ptr<Arrow> {
        return std::make_shared<Arrow>(params);
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