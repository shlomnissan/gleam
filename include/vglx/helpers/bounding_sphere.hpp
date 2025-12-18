/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/math/color.hpp"
#include "vglx/math/sphere.hpp"
#include "vglx/scene/node.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Visual debug helper that renders the bounds of a spherical volume.
 *
 * BoundingSphere displays a wireframe sphere representing the extents of a
 * @ref Sphere. It is useful for inspecting spatial queries, validating geometry
 * bounds, or visualizing collision volumes. The sphere is rendered as a simple
 * line mesh colored as specified.
 *
 * @code
 * auto geometry = vglx::BoxGeometry::Create();
 * auto bounds = vglx::BoundingSphere::Create(
 *   geometry->BoundingSphere(),
 *   0xFF0000
 * );
 *
 * my_scene->Add(bounds);
 * @endcode
 *
 * @ingroup HelpersGroup
 */
class VGLX_EXPORT BoundingSphere : public Node {
public:
    /**
     * @brief Constructs a bounding-sphere debug node.
     *
     * @param sphere Spherical bounds to visualize.
     * @param color Line color used for rendering the wireframe sphere.
     */
    BoundingSphere(const Sphere& sphere, const Color& color);

    /**
     * @brief Creates an instance of @ref BoundingSphere.
     *
     * @param sphere Spherical bounds to visualize.
     * @param color Line color used for rendering.
     */
    [[nodiscard]] static auto
    Create(const Sphere& sphere, const Color& color) -> std::unique_ptr<BoundingSphere> {
        return std::make_unique<BoundingSphere>(sphere, color);
    }
};

}