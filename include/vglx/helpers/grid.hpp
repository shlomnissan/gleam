/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/math/color.hpp"
#include "vglx/scene/mesh.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Visual debug helper that renders a ground-aligned reference grid.
 *
 * Grid draws a square grid centered at the origin, aligned with the XZ-plane.
 * It is typically used as a spatial reference when inspecting object placement,
 * world scale, or camera movement. The grid consists of evenly spaced lines
 * whose extent, color, and subdivision count can be customized.
 *
 * @code
 * my_scene->Add(vglx::Grid::Create({
 *   .color = 0x333333u,
 *   .size = 4.0f,
 *   .divisions = 16
 * }));
 * @endcode
 *
 * @ingroup HelpersGroup
 */
class VGLX_EXPORT Grid : public Mesh {
public:
    /**
     * @brief Parameters for constructing a @ref Grid object.
     */
    struct Parameters {
        Color color {0x333333u}; ///< Line color used for rendering.
        float size {4.0f}; ///< Half-width of the grid in world units.
        unsigned int divisions {16}; ///< Number of grid subdivisions along each axis.
    };

    /**
     * @brief Constructs a grid node.
     *
     * @param params @ref Grid::Parameters "Initialization parameters"
     * for constructing the grid.
     */
    explicit Grid(const Parameters& params);

    /**
     * @brief Creates an instance of @ref Grid with default parameters.
     */
    [[nodiscard]] static auto Create() -> std::unique_ptr<Grid> {
        return std::make_unique<Grid>(Parameters {});
    }

    /**
     * @brief Creates an instance of @ref Grid.
     *
     * @param params @ref Grid::Parameters "Initialization parameters"
     * for constructing the grid.
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::unique_ptr<Grid> {
        return std::make_unique<Grid>(params);
    }
};

}