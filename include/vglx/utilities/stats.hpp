/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include <memory>

namespace vglx {

/**
 * @brief Collects and visualizes runtime performance statistics.
 *
 * This class tracks frames per second, frame time, and the number of rendered
 * objects per frame. Use it in your main loop to provide an on-screen
 * performance overlay during development and debugging.
 *
 * @code
 * while (running) {
 *   stats.BeforeRender();
 *   renderer.Render(scene, camera);
 *   stats.AfterRender(renderer.RenderedObjectsPerFrame());
 *   stats.Draw();
 * }
 * @endcode
 *
 * This overlay currently requires [ImGui](https://github.com/ocornut/imgui) support.
 * If the engine is not compiled with `VGLX_USE_IMGUI`, the @ref Draw method
 * becomes a no-op.
 *
 * @ingroup UtilitiesGroup
 */
class VGLX_EXPORT Stats {
public:
    /**
     * @brief Constructs a stats object.
     */
    Stats();

    /**
     * @brief Marks the beginning of a frame render.
     *
     * Records timing information and increments the frame counter. Call this
     * before issuing any render commands.
     */
    auto BeforeRender() -> void;

    /**
     * @brief Marks the end of a frame render.
     *
     * Updates frame time and records the number of rendered objects. The number
     * of objects can be retrieved from the
     * @ref Renderer::RenderedObjectsPerFrame "renderer".
     *
     * @param n_objects Number of objects rendered in the frame.
     */
    auto AfterRender(unsigned n_objects) -> void;

    /**
     * @brief Draws the performance overlay.
     *
     * Renders a window containing FPS, frame time, and rendered object
     * histograms.
     */
    auto Draw() const -> void;

    ~Stats();

private:
    /// @cond INTERNAL
    class Impl;
    std::unique_ptr<Impl> impl_;
    /// @endcond
};

}