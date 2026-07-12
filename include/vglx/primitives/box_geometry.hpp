/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/geometries/geometry.hpp"
#include "vglx/math/vector3.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Generated geometry representing a box.
 *
 * BoxGeometry generates a mesh for an axis-aligned box defined by its width,
 * height, and depth. Each dimension can be subdivided using segment counts
 * to control vertex density and support smoother lighting or deformation.
 * The geometry is constructed in local space, centered at the origin.
 *
 * @code
 * auto geometry = vglx::BoxGeometry::Create({
 *   .width = 2.0f,
 *   .height = 1.0f,
 *   .depth = 3.0f,
 *   .width_segments = 2,
 *   .height_segments = 2,
 *   .depth_segments = 2
 * });
 *
 * auto material = vglx::PhongMaterial::Create({.color = 0x049EF4});
 *
 * my_scene->Add(vglx::Mesh::Create(geometry, material));
 * @endcode
 *
 * @ingroup PrimitivesGroup
 */
class VGLX_EXPORT BoxGeometry : public Geometry {
public:
    /// @brief Parameters for constructing a @ref BoxGeometry object.
    struct Parameters {
        float width {1.0f}; ///< Size along the X-axis.
        float height {1.0f}; ///< Size along the Y-axis.
        float depth {1.0f}; ///< Size along the Z-axis.
        unsigned width_segments {1}; ///< Subdivisions along X.
        unsigned height_segments {1}; ///< Subdivisions along Y.
        unsigned depth_segments {1}; ///< Subdivisions along Z.
    };

    /**
     * @brief Constructs a box geometry.
     *
     * @param params @ref BoxGeometry::Parameters "Initialization parameters"
     * for constructing the geometry.
     */
    explicit BoxGeometry(const Parameters& params);

    /**
     * @brief Creates a shared instance of @ref BoxGeometry with default parameters.
     */
    [[nodiscard]] static auto Create() -> std::shared_ptr<BoxGeometry> {
        return std::make_shared<BoxGeometry>(Parameters {});
    }

    /**
     * @brief Creates a shared instance of @ref BoxGeometry with custom parameters.
     *
     * @param params @ref BoxGeometry::Parameters "Initialization parameters"
     * for constructing the geometry.
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::shared_ptr<BoxGeometry> {
        return std::make_shared<BoxGeometry>(params);
    }
};

}
