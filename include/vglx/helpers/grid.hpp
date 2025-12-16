/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/math/color.hpp"
#include "vglx/scene/node.hpp"

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
 * auto grid = vglx::Grid::Create({
 *   .color = 0x333333,
 *   .size = 4.0f,
 *   .divisions = 16
 * });
 *
 * my_scene->Add(grid);
 * @endcode
 *
 * @ingroup HelpersGroup
 */
class VGLX_EXPORT Grid : public Node {
public:
    /**
     * @brief Parameters for constructing a @ref Grid object.
     */
    struct Parameters {
        Color color; ///< Line color used for rendering.
        float size; ///< Half-width of the grid in world units.
        unsigned int divisions; ///< Number of grid subdivisions along each axis.
    };

    /**
     * @brief Constructs a grid node.
     *
     * @param params @ref Grid::Parameters "Initialization parameters"
     * for constructing the grid.
     */
    explicit Grid(const Parameters& params);

    /**
     * @brief Creates a shared instance of @ref Grid.
     *
     * @param params @ref Grid::Parameters "Initialization parameters"
     * for constructing the grid.
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::shared_ptr<Grid> {
        return std::make_shared<Grid>(params);
    }
};

}