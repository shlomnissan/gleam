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
#include "vglx/scene/mesh.hpp"

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
 * my_scene->Add(vglx::BoundingSphere::Create({
 *   .sphere = geometry->BoundingSphere(),
 *   .color = 0xFF0000u
 * }));
 * @endcode
 *
 * @ingroup HelpersGroup
 */
class VGLX_EXPORT BoundingSphere : public Mesh {
public:
    /// @brief Parameters for constructing a @ref BoundingSphere object.
    struct Parameters {
        Sphere sphere; ///< Spherical bounds to visualize.
        Color color {0xFFFFFFu}; ///< Line color used for rendering the wireframe sphere.
    };

    /**
     * @brief Constructs a bounding-sphere debug node.
     *
     * @param params @ref BoundingSphere::Parameters "Initialization parameters"
     * for constructing the bounding sphere.
     */
    explicit BoundingSphere(const Parameters& params);

    /**
     * @brief Creates an instance of @ref BoundingSphere.
     *
     * @param params @ref BoundingSphere::Parameters "Initialization parameters"
     * for constructing the bounding sphere.
     */
    [[nodiscard]] static auto
    Create(const Parameters& params) -> std::unique_ptr<BoundingSphere> {
        return std::make_unique<BoundingSphere>(params);
    }
};

}