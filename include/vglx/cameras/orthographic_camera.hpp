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
 * In this projection mode, an object's size in the rendered image stays
 * constant regardless of its distance from the camera. This can be useful for
 * rendering 2D scenes and UI elements, amongst other things.
 *
 * Although multiple cameras can be added to the scene graph and inherit
 * transformations from their parent objects, only one camera can be active at
 * a time. The active camera is managed by the application’s runtime object:
 *
 * @code
 * class MyApp : public vglx::Application {
 * public:
 *   auto Configure() -> void override {}
 *
 *   auto Setup() -> void override {
 *     SetCamera(
 *       vglx::OrthographicCamera::Create({
 *         .left = 0.0f,
 *         .right = 1024.0f,
 *         .top = 0.0f,
 *         .bottom = 768.0f,
 *         .near = 0.1f,
 *         .far = 100.0f
 *       })
 *     );
 *   }
 *
 *   auto Update(float delta) -> bool override {
 *     return true;
 *   }
 * }
 * @endcode
 *
 * @ingroup CamerasGroup
 */
class VGLX_EXPORT OrthographicCamera : public Camera {
public:
    /// @brief Parameters for constructing an @ref OrthographicCamera object.
    struct Parameters {
        float left; ///< Left clipping plane.
        float right; ///< Right clipping plane.
        float top; ///< Top clipping plane.
        float bottom; ///< Bottom clipping plane.
        float near; ///< Near clipping plane.
        float far; ///< Far clipping plane.
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
     * @brief Creates a shared instance of @ref OrthographicCamera.
     *
     * @param params @ref OrthographicCamera::Parameters "Initialization parameters"
     * for constructing the camera.
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::shared_ptr<OrthographicCamera> {
        return std::make_shared<OrthographicCamera>(params);
    }

    /**
     * @brief Updates the projection transform to match the new viewport size.
     *
     * The behavior depends on the current resize policy.
     *
     * @param width Viewport width in pixels.
     * @param height Viewport height in pixels.
     */
    auto Resize(int width, int height) -> void override;

private:
    /// @cond INTERNAL
    Parameters params_;
    /// @endcond
};

}