/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/geometries/geometry.hpp"
#include "vglx/math/utilities.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Generated geometry representing a sphere.
 *
 * SphereGeometry constructs a UV-parameterized sphere centered at the origin.
 * The radius defines its size, while segmentation properties control tessellation
 * along longitude and latitude. Higher segment counts produce smoother lighting
 * and silhouettes at the cost of additional vertices.
 *
 * @code
 * auto geometry = vglx::SphereGeometry::Create({
 *     .radius = 1.0f,
 *     .width_segments = 64,
 *     .height_segments = 32
 * });
 *
 * auto material = vglx::PhongMaterial::Create({.color = 0x049EF4});
 *
 * my_scene->Add(vglx::Mesh::Create(geometry, material));
 * @endcode
 *
 * @ingroup PrimitivesGroup
 */
class VGLX_EXPORT SphereGeometry : public Geometry {
public:
    /// @brief Parameters for constructing a @ref SphereGeometry object.
    struct Parameters {
        float radius {1.0f}; ///< Radius of the sphere.
        unsigned width_segments {32}; ///< Segments around the circumference (longitude).
        unsigned height_segments {16}; ///< Segments from top to bottom (latitude).
    };

    /**
     * @brief Constructs a sphere geometry.
     *
     * @param params @ref SphereGeometry::Parameters "Initialization parameters"
     * that control the size and tessellation of the sphere.
     */
    explicit SphereGeometry(const Parameters& params);

    /**
     * @brief Creates a shared instance of @ref SphereGeometry with default parameters.
     */
    [[nodiscard]] static auto Create() -> std::shared_ptr<SphereGeometry> {
        return std::make_shared<SphereGeometry>(Parameters {});
    }

    /**
     * @brief Creates a shared instance of @ref SphereGeometry with custom parameters.
     *
     * @param params @ref SphereGeometry::Parameters "Initialization parameters"
     * used to generate the sphere.
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::shared_ptr<SphereGeometry> {
        return std::make_shared<SphereGeometry>(params);
    }
};

}