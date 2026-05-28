/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/geometries/geometry.hpp"

namespace vglx {

/**
 * @brief Generated geometry representing the wireframe edges of a triangle mesh.
 *
 * `WireframeGeometry` takes an existing triangle-based geometry and expands its
 * indexed triangle list into a line list suitable for rendering with
 * @ref Geometry::PrimitiveType "Geometry::PrimitiveType::Lines". The original vertex buffer is reused,
 * while the index buffer is replaced with pairs of indices representing each
 * unique edge of the mesh.
 *
 * This is useful for visual debugging (visualizing topology, silhouette edges,
 * or bounding structures) without modifying the source geometry.
 *
 * @note The input geometry must provide valid triangle indices; non-indexed
 * or non-triangle primitives are not supported.
 *
 * @code
 * auto solid = vglx::BoxGeometry::Create();
 * auto wireframe  = vglx::WireframeGeometry::Create(solid.get());
 * auto material = vglx::UnlitMaterial::Create({.color = 0xFFFFFF});
 *
 * my_scene->Add(vglx::Mesh::Create(wireframe, material));
 * @endcode
 *
 * @ingroup GeometryGroup
 */
class VGLX_EXPORT WireframeGeometry : public Geometry {
public:
    /**
     * @brief Constructs a wireframe representation of an existing geometry.
     *
     * The vertex data is shared, but the index buffer is rebuilt so that each
     * triangle contributes three line segments.
     *
     * @param geometry Pointer to the original triangle-based geometry.
     */
    explicit WireframeGeometry(const Geometry* geometry);

    /**
     * @brief Creates a shared instance of @ref WireframeGeometry.
     *
     * @param geometry Pointer to the original triangle-based geometry.
     */
    [[nodiscard]] static auto Create(const Geometry* geometry) -> std::shared_ptr<WireframeGeometry> {
        return std::make_shared<WireframeGeometry>(geometry);
    }
};

}