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
#include "vglx/scene/mesh.hpp"

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
 * my_scene->Add(vglx::BoundingBox::Create({
 *   .box = geometry->BoundingBox(),
 *   .color = 0xFFFFFFu
 * }));
 * @endcode
 *
 * @ingroup HelpersGroup
 */
class VGLX_EXPORT BoundingBox : public Mesh {
public:
    /// @brief Parameters for constructing a @ref BoundingBox object.
    struct Parameters {
        Box3 box; ///< Axis-aligned bounds to visualize.
        Color color {0xFFFFFFu}; ///< Line color used for rendering the box.
    };

    /**
     * @brief Constructs a bounding box debug node.
     *
     * @param params @ref BoundingBox::Parameters "Initialization parameters"
     * for constructing the bounding box.
     */
    explicit BoundingBox(const Parameters& params);

    /**
     * @brief Creates an instance of @ref BoundingBox.
     *
     * @param params @ref BoundingBox::Parameters "Initialization parameters"
     * for constructing the bounding box.
     */
    [[nodiscard]] static auto
    Create(const Parameters& params) -> std::unique_ptr<BoundingBox> {
        return std::make_unique<BoundingBox>(params);
    }
};

}