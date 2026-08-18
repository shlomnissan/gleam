/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/math/color.hpp"
#include "vglx/math/plane.hpp"
#include "vglx/scene/node.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Visual debug helper that renders a finite preview of an infinite plane.
 *
 * BoundingPlane draws a square region aligned with the given @ref Plane.
 * Since mathematical planes extend infinitely, this helper renders a finite,
 * centered patch of configurable size, useful for visualizing clipping planes,
 * ground planes, or intersection tests during debugging. The patch is drawn as
 * a simple wireframe in the color provided.
 *
 * @code
 * my_scene->Add(vglx::BoundingPlane::Create({
 *   .plane = vglx::Plane {vglx::Vector3::Z(), 0.0f},
 *   .size = 2.0f,
 *   .color = 0xFF0000u
 * }));
 * @endcode
 *
 * @ingroup HelpersGroup
 */
class VGLX_EXPORT BoundingPlane : public Node {
public:
    /// @brief Parameters for constructing a @ref BoundingPlane object.
    struct Parameters {
        Plane plane; ///< Infinite geometric plane to visualize.
        float size {1.0f}; ///< Extent of the rendered square patch, measured from center to edge.
        Color color {0xFFFFFFu}; ///< Line color used to draw the patch.
    };

    /**
     * @brief Constructs a bounding plane debug node.
     *
     * @param params @ref BoundingPlane::Parameters "Initialization parameters"
     * for constructing the bounding plane.
     */
    explicit BoundingPlane(const Parameters& params);

    /**
     * @brief Creates an instance of @ref BoundingPlane.
     *
     * @param params @ref BoundingPlane::Parameters "Initialization parameters"
     * for constructing the bounding plane.
     */
    [[nodiscard]] static auto
    Create(const Parameters& params) -> std::unique_ptr<BoundingPlane> {
        return std::make_unique<BoundingPlane>(params);
    }
};

}