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
#include "vglx/nodes/node.hpp"

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
 * auto plane  = vglx::Plane {vglx::Vector3::Forward(), 0.0f};
 * auto bounds = vglx::BoundingPlane::Create(plane, 2.0f, 0xFF0000);
 *
 * my_scene->Add(bounds);
 * @endcode
 *
 * @ingroup HelpersGroup
 */
class VGLX_EXPORT BoundingPlane : public Node {
public:
    /**
     * @brief Constructs a bounding plane debug node.
     *
     * @param plane Infinite geometric plane to visualize.
     * @param size Extent of the rendered square patch, measured from center to edge.
     * @param color Line color used to draw the patch.
     */
    BoundingPlane(const Plane& plane, float size, const Color& color);

    /**
     * @brief Creates a shared instance of @ref BoundingPlane.
     *
     * @param plane Infinite geometric plane to visualize.
     * @param size Extent of the rendered square patch.
     * @param color Line color used to draw the patch.
     */
    [[nodiscard]] static auto
    Create(const Plane& plane, float size, const Color& color) -> std::shared_ptr<BoundingPlane> {
        return std::make_shared<BoundingPlane>(plane, size, color);
    }
};

}