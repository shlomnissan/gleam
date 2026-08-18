/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/geometries/geometry.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Generated geometry representing a torus knot.
 *
 * TorusKnotGeometry constructs a mesh for a (p, q) torus knot, a closed
 * curve that winds @c p times around the torus's axis of rotational
 * symmetry and @c q times around a circle in its interior. The tube is
 * extruded along this curve using a Frenet-style frame. When @c p and
 * @c q are coprime the result is a true knot; otherwise it forms a link
 * of multiple loops.
 *
 * @code
 * auto geometry = vglx::TorusKnotGeometry::Create({
 *     .radius = 1.0f,
 *     .tube = 0.4f,
 *     .tubular_segments = 64,
 *     .radial_segments = 8,
 *     .p = 2,
 *     .q = 3
 * });
 *
 * auto material = vglx::PhongMaterial::Create({.color = 0xFF6E00u});
 *
 * my_scene->Add(vglx::Mesh::Create(geometry, material));
 * @endcode
 *
 * @ingroup PrimitivesGroup
 */
class VGLX_EXPORT TorusKnotGeometry : public Geometry {
public:
    /// @brief Parameters for constructing a @ref TorusKnotGeometry object.
    struct Parameters {
        float radius {1.0f}; ///< Radius of the torus the knot is wound around.
        float tube {0.4f}; ///< Radius of the tube extruded along the curve.
        unsigned tubular_segments {64}; ///< Segments along the length of the curve.
        unsigned radial_segments {8}; ///< Segments around the tube cross-section.
        unsigned p {2}; ///< Number of windings around the axis of rotational symmetry.
        unsigned q {3}; ///< Number of windings around the interior of the torus.
    };

    /**
     * @brief Constructs a torus knot geometry.
     *
     * @param params @ref TorusKnotGeometry::Parameters "Initialization parameters"
     * that control the shape and tessellation of the torus knot.
     */
    explicit TorusKnotGeometry(const Parameters& params);

    /**
     * @brief Creates a shared instance of @ref TorusKnotGeometry with default parameters.
     */
    [[nodiscard]] static auto Create() -> std::shared_ptr<TorusKnotGeometry> {
        return std::make_shared<TorusKnotGeometry>(Parameters {});
    }

    /**
     * @brief Creates a shared instance of @ref TorusKnotGeometry with custom parameters.
     *
     * @param params @ref TorusKnotGeometry::Parameters "Initialization parameters"
     * used to generate the torus knot.
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::shared_ptr<TorusKnotGeometry> {
        return std::make_shared<TorusKnotGeometry>(params);
    }
};

}
