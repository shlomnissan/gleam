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
 * @brief Generated geometry representing a 3D cylinder.
 *
 * CylinderGeometry constructs a mesh for a cylinder aligned along the +Y axis.
 * It supports differing top and bottom radii (allowing tapered shapes), an
 * adjustable height, optional subdivision along both radial and vertical
 * directions, and the option to render the cylinder with or without end caps.
 * The geometry is centered in local space, with its midpoint at the origin.
 *
 * @code
 * auto geometry = vglx::CylinderGeometry::Create({
 *   .radius_top = 0.8f,
 *   .radius_bottom = 1.0f,
 *   .height = 2.5f,
 *   .radial_segments = 32,
 *   .height_segments = 3,
 *   .open_ended = false
 * });
 *
 * auto material = vglx::PhongMaterial::Create(0x8BC34A);
 *
 * my_scene->Add(vglx::Mesh::Create(geometry, material));
 * @endcode
 *
 * @ingroup PrimitivesGroup
 */
class VGLX_EXPORT CylinderGeometry : public Geometry {
public:
    /// @brief Parameters for constructing a @ref CylinderGeometry object.
    struct Parameters {
        float radius_top {1.0f}; ///< Radius of the top cap.
        float radius_bottom {1.0f}; ///< Radius of the bottom cap.
        float height {1.0f}; ///< Height along the +Y axis.
        unsigned int radial_segments {32}; ///< Number of radial subdivisions.
        unsigned int height_segments {1}; ///< Number of vertical subdivisions.
        bool open_ended {false}; ///< If true, the end caps are omitted.
    };

    /**
     * @brief Constructs a cylinder geometry.
     *
     * @param params @ref CylinderGeometry::Parameters "Initialization parameters"
     * for constructing the geometry.
     */
    explicit CylinderGeometry(const Parameters& params);

    /**
     * @brief Creates a shared instance of @ref CylinderGeometry with default parameters.
     */
    [[nodiscard]] static auto Create() -> std::shared_ptr<CylinderGeometry> {
        return std::make_shared<CylinderGeometry>(Parameters {});
    }

    /**
     * @brief Creates a shared instance of @ref CylinderGeometry with custom parameters.
     *
     * @param params @ref CylinderGeometry::Parameters "Initialization parameters"
     * for constructing the geometry.
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::shared_ptr<CylinderGeometry> {
        return std::make_shared<CylinderGeometry>(params);
    }
};

}