/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/cameras/camera.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Represents a camera that uses perspective projection.
 *
 * This projection mode is designed to mimic the way the human eye sees. It is
 * the most common projection mode used for rendering a 3D scene.
 *
 * Although multiple cameras can be added to the scene graph and inherit
 * transformations from their parent objects, only one camera is rendered at
 * a time, the one passed to @ref Renderer::Render. Construct the camera
 * directly with the window's aspect ratio and pass it into the render call
 * each frame.
 *
 * @code
 * auto camera = vglx::PerspectiveCamera::Create({
 *   .fov = vglx::math::DegToRad(60.0f),
 *   .aspect = window.AspectRatio(),
 *   .near = 0.1f,
 *   .far = 1000.0f
 * });
 *
 * renderer.Render(scene.get(), camera.get());
 * @endcode
 *
 * @ingroup CamerasGroup
 */
class VGLX_EXPORT PerspectiveCamera : public Camera {
public:
    /// @brief Parameters for constructing a @ref PerspectiveCamera object.
    struct Parameters {
        float fov; ///< Vertical field of view in radians.
        float aspect; ///< Aspect ratio.
        float near; ///< Distance to the near clipping plane.
        float far; ///< Distance to the far clipping plane.
    };

    /**
     * @brief Constructs a perspective camera.
     *
     * @param params @ref PerspectiveCamera::Parameters "Initialization parameters"
     * for constructing the camera.
     */
    explicit PerspectiveCamera(const Parameters& params);

    /**
     * @brief Creates an instance of @ref PerspectiveCamera.
     *
     * @param params @ref PerspectiveCamera::Parameters "Initialization parameters"
     * for constructing the camera.
     */
    [[nodiscard]] static auto
    Create(const Parameters& params) -> std::unique_ptr<PerspectiveCamera> {
        return std::make_unique<PerspectiveCamera>(params);
    }

    /**
     * @brief Updates the projection transform to match the new viewport size.
     *
     * @param width Viewport width in pixels.
     * @param height Viewport height in pixels.
     */
    auto Resize(int width, int height) -> void override;

    /**
     * @brief Configures perspective projection parameters.
     *
     * Updates the camera's vertical field of view, near plane, and far plane,
     * and rebuilds the projection transform accordingly. The aspect ratio
     * remains unchanged until @ref Resize is called.
     *
     * @param fov  Vertical field of view in radians.
     * @param near Distance to the near clipping plane.
     * @param far  Distance to the far clipping plane.
     */
    auto SetLens(float fov, float near, float far) -> void;

private:
    /// @cond INTERNAL
    Parameters params_;
    /// @endcond
};

}