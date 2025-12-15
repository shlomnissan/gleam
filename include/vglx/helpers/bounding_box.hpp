/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/math/box3.hpp"
#include "vglx/math/color.hpp"
#include "vglx/nodes/node.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Visual debug helper that renders the axis-aligned bounds of an object.
 *
 * BoundingBox displays a wireframe box representing the extents of a
 * @ref Box3 volume. It is intended for debugging spatial queries, culling
 * behavior, and validating geometry bounds. The box is rendered as a simple
 * line mesh in the color provided.
 *
 * @code
 * auto geometry = vglx::SphereGeometry::Create();
 * auto bounds = vglx::BoundingBox::Create(
 *   geometry->BoundingBox(),
 *   0xFFFFFF
 * );
 * my_scene->Add(bounds);
 * @endcode
 *
 * @ingroup HelpersGroup
 */
class VGLX_EXPORT BoundingBox : public Node {
public:
    /**
     * @brief Constructs a bounding box debug node.
     *
     * @param box Axis-aligned bounds to visualize.
     * @param color Line color used for rendering the box.
     */
    BoundingBox(const Box3& box, const Color& color);

    /**
     * @brief Creates a shared instance of @ref BoundingBox.
     *
     * @param box Axis-aligned bounds to visualize.
     * @param color Line color used for rendering the box.
     */
    [[nodiscard]] static auto
    Create(const Box3& box, const Color& color) -> std::shared_ptr<BoundingBox> {
        return std::make_shared<BoundingBox>(box, color);
    }
};

}