/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/cameras/camera.hpp"
#include "vglx/scene/node.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Interactive camera controller for orbit-style navigation.
 *
 * OrbitControls lets the user rotate, pan, and zoom a camera around a
 * target point using mouse input. It is added to the scene like any other
 * node and updates the attached camera each frame. The controller maintains
 * a spherical orbit radius and angular orientation
 * @ref OrbitControls::Parameters "pitch" and @ref OrbitControls::Parameters "yaw"
 * and applies smoothing when a non-zero damping factor is set.
 *
 * The control scheme includes:
 * - Left mouse drag: orbit around the target
 * - Right mouse drag: pan in view space
 * - Scroll wheel: zoom in and out
 *
 * @code
 * auto MyScene::OnAttached(SharedContextPointer context) -> void override {
 *   Add(vglx::OrbitControls::Create(
 *     context->camera, {
 *       .radius = 5.0f
 *     }
 *   ));
 * }
 * @endcode
 *
 * @ingroup HelpersGroup
 */
class VGLX_EXPORT OrbitControls : public Node {
public:
    /// @brief Parameters for constructing an @ref OrbitControls object.
    struct Parameters {
        float radius {1.0f}; ///< Initial distance of the camera from its target.
        float pitch {0.0f}; ///< Initial pitch angle in radians.
        float yaw {0.0f}; ///< Initial yaw angle in radians.
        float orbit_speed {0.005f}; ///< Mouse sensitivity when orbiting.
        float pan_speed {0.002f}; ///< Mouse sensitivity when panning.
        float zoom_speed {0.95f}; ///< Scroll wheel zoom factor.
        float damping_factor {0.3f}; ///< Set to 1 for instant response.
    };

    /**
     * @brief Constructs orbit controls.
     *
     * @param camera Camera the controller manipulates.
     * @param params Configuration parameters.
     */
    OrbitControls(Camera* camera, const Parameters& params);

    /**
     * @brief Creates an instance of @ref OrbitControls.
     *
     * @param camera Camera the controller manipulates.
     * @param params Configuration parameters.
     */
    [[nodiscard]] static auto
    Create(Camera* camera, const Parameters& params) -> std::unique_ptr<OrbitControls> {
        return std::make_unique<OrbitControls>(camera, params);
    }

    /**
     * @brief Responds to mouse input for orbiting, panning, and zooming.
     *
     * @param event Mouse event pointer.
     */
    auto OnMouseEvent(MouseEvent* event) -> void override;

    /**
     * @brief Updates camera position each frame, applying damping if enabled.
     *
     * @param delta Time step in seconds.
     */
    auto OnUpdate(float delta) -> void override;

    ~OrbitControls() override;

private:
    /// @cond INTERNAL
    class Impl;
    std::unique_ptr<Impl> impl_;
    /// @endcond
};

}