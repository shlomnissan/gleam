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
 * @brief Generated geometry representing a flat 2D plane.
 *
 * PlaneGeometry creates a rectangular, axis-aligned plane centered at the origin
 * in local space. Its width and height define the plane’s extents along the X and Y
 * axes, and optional subdivision counts allow the plane to be tessellated for
 * deformation, smooth shading, or procedural effects.
 *
 * @code
 * auto geometry = vglx::PlaneGeometry::Create({
 *     .width = 5.0f,
 *     .height = 3.0f,
 *     .width_segments = 2,
 *     .height_segments = 2
 * });
 *
 * auto material = vglx::PhongMaterial::Create({.color = 0x049EF4});
 *
 * my_scene->Add(vglx::Mesh::Create(geometry, material));
 * @endcode
 *
 * @ingroup PrimitivesGroup
 */
class VGLX_EXPORT PlaneGeometry : public Geometry {
public:
    /// @brief Parameters for constructing a @ref PlaneGeometry object.
    struct Parameters {
        float width {1.0f}; ///< Size along the X-axis.
        float height {1.0f}; ///< Size along the Y-axis.
        unsigned width_segments {1}; ///< Subdivisions along the plane's width.
        unsigned height_segments {1}; ///< Subdivisions along the plane's height.
    };

    /**
     * @brief Constructs a plane geometry.
     *
     * @param params @ref PlaneGeometry::Parameters "Initialization parameters"
     * used to generate the plane.
     */
    explicit PlaneGeometry(const Parameters& params);

    /**
     * @brief Creates a shared instance of @ref PlaneGeometry with default parameters.
     */
    [[nodiscard]] static auto Create() -> std::shared_ptr<PlaneGeometry> {
        return std::make_shared<PlaneGeometry>(Parameters {});
    }

    /**
     * @brief Creates a shared instance of @ref PlaneGeometry with custom parameters.
     *
     * @param params @ref PlaneGeometry::Parameters "Initialization parameters"
     * used to generate the plane.
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::shared_ptr<PlaneGeometry> {
        return std::make_shared<PlaneGeometry>(params);
    }
};

}