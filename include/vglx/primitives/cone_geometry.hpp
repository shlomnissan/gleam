/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/geometries/geometry.hpp"
#include "vglx/primitives/cylinder_geometry.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Generated geometry representing a cone.
 *
 * ConeGeometry constructs a mesh for a cone aligned along the +Y axis. It is
 * defined by a radius at the base, a height, optional segmentation along both
 * radial and vertical directions, and an option to create an open or closed
 * bottom cap. The geometry is centered in local space with its tip at +Y.
 *
 * @code
 * auto geometry = vglx::ConeGeometry::Create({
 *   .radius = 0.5f,
 *   .height = 2.0f,
 *   .radial_segments = 32,
 *   .height_segments = 4,
 *   .open_ended = false
 * });
 *
 * auto material = vglx::PhongMaterial::Create({.color = 0xF2B632u});
 *
 * my_scene->Add(vglx::Mesh::Create(geometry, material));
 * @endcode
 *
 * @ingroup PrimitivesGroup
 */
class VGLX_EXPORT ConeGeometry : public CylinderGeometry {
public:
    /// @brief Parameters for constructing a @ref ConeGeometry object.
    struct Parameters {
        float radius {1.0f}; ///< Base radius of the cone.
        float height {1.0f}; ///< Height along the +Y axis.
        unsigned radial_segments {16}; ///< Number of radial subdivisions.
        unsigned height_segments {1}; ///< Number of vertical subdivisions.
        bool open_ended {false}; ///< If true, the bottom cap is omitted.
    };

    /**
     * @brief Constructs a cone geometry.
     *
     * @param params @ref ConeGeometry::Parameters "Initialization parameters"
     * for constructing the geometry.
     */
    explicit ConeGeometry(const Parameters& params);

    /**
     * @brief Creates a shared instance of @ref ConeGeometry with default parameters.
     */
    [[nodiscard]] static auto Create() -> std::shared_ptr<ConeGeometry> {
        return std::make_shared<ConeGeometry>(Parameters {});
    }

    /**
     * @brief Creates a shared instance of @ref ConeGeometry with custom parameters.
     *
     * @param params @ref ConeGeometry::Parameters "Initialization parameters"
     * for constructing the geometry.
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::shared_ptr<ConeGeometry> {
        return std::make_shared<ConeGeometry>(params);
    }
};

}