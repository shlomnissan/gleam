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
 * @brief Represents a camera that uses orthographic projection.
 *
 * In this projection mode an object's size in the rendered image stays
 * constant regardless of its distance from the camera. This can be useful for
 * rendering 2D scenes and UI elements, amongst other things.
 *
 * Although multiple cameras can be added to the scene graph and inherit
 * transformations from their parent objects, only one camera is rendered at
 * a time, the one passed to @ref Renderer::Render. Construct it directly
 * and pass it into the render call each frame.
 *
 * @code
 * auto camera = vglx::OrthographicCamera::Create({
 *   .left = 0.0f,
 *   .right = 1280.0f,
 *   .top = 0.0f,
 *   .bottom = 720.0f,
 *   .near = 0.1f,
 *   .far = 100.0f
 * });
 *
 * renderer.Render(scene.get(), camera.get());
 * @endcode
 *
 * @ingroup CamerasGroup
 */
class VGLX_EXPORT OrthographicCamera : public Camera {
public:
    /// @brief Parameters for constructing an @ref OrthographicCamera object.
    struct Parameters {
        float left {-1.0f}; ///< Left clipping plane.
        float right {1.0f}; ///< Right clipping plane.
        float top {1.0f}; ///< Top clipping plane.
        float bottom {-1.0f}; ///< Bottom clipping plane.
        float near {0.1f}; ///< Near clipping plane.
        float far {1000.0f}; ///< Far clipping plane.
    };

    /**
     * @brief Defines how the orthographic projection should adapt on window resize.
     */
    enum class ResizePolicy {
        PixelSpace, ///< Projection matches framebuffer dimensions.
        FixedVertical, ///< Vertical extent remains constant, horizontal adjusted.
        FixedHorizontal ///< Horizontal extent remains constant, vertical adjusted.
    };

    /// @brief The current resize policy for this camera.
    ResizePolicy resize_policy = ResizePolicy::PixelSpace;

   /**
     * @brief Constructs an orthographic camera.
     *
     * @param params @ref OrthographicCamera::Parameters "Initialization parameters"
     * for constructing the camera.
     */
    explicit OrthographicCamera(const Parameters& params);

    /**
     * @brief Creates an instance of @ref OrthographicCamera with default parameters.
     */
    [[nodiscard]] static auto Create() -> std::unique_ptr<OrthographicCamera> {
        return std::make_unique<OrthographicCamera>(Parameters {});
    }

    /**
     * @brief Creates an instance of @ref OrthographicCamera.
     *
     * @param params @ref OrthographicCamera::Parameters "Initialization parameters"
     * for constructing the camera.
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::unique_ptr<OrthographicCamera> {
        return std::make_unique<OrthographicCamera>(params);
    }

    /**
     * @brief Configures orthographic projection parameters.
     *
     * Replaces the camera's clipping planes and rebuilds the projection
     * transform accordingly.
     *
     * @param params @ref OrthographicCamera::Parameters "Projection parameters"
     * defining the new clipping planes.
     */
    auto SetLens(const Parameters& params) -> void;

protected:
    /**
     * @brief Updates the projection transform to match the new viewport size.
     *
     * The behavior depends on the current resize policy.
     *
     * @param width Viewport width in pixels.
     * @param height Viewport height in pixels.
     */
    auto OnResize(int width, int height) -> void override;

private:
    /// @cond INTERNAL
    Parameters params_;
    /// @endcond
};

}