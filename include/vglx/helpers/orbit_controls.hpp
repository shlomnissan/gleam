/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/cameras/camera.hpp"
#include "vglx/math/utilities.hpp"
#include "vglx/math/vector3.hpp"
#include "vglx/scene/node.hpp"

#include <limits>
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
 * - Right mouse drag or Shift + left mouse drag: pan in view space
 * - Scroll wheel: zoom in and out
 *
 * @code
 * struct MyScene : public vglx::Scene {
 *   MyScene(vglx::Camera* camera) {
 *     Add(vglx::OrbitControls::Create(camera, {
 *       .radius = 5.0f
 *     }));
 *   }
 * };
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
        Vector3 target {Vector3::Zero()}; ///< Initial point the camera orbits around.
        float orbit_speed {3.0f}; ///< Orbit sensitivity in radians per viewport-height drag.
        float pan_speed {1.0f}; ///< Pan sensitivity. At 1 the target tracks the cursor exactly.
        float zoom_speed {1.0f}; ///< Scroll wheel zoom sensitivity multiplier.
        float damping_factor {0.3f}; ///< Set to 1 for instant response.
        float min_distance {0.1f}; ///< Minimum orbit distance from the target.
        float max_distance {std::numeric_limits<float>::infinity()}; ///< Maximum orbit distance from the target.
        float min_pitch {-math::pi_over_2}; ///< Minimum pitch angle in radians.
        float max_pitch {math::pi_over_2}; ///< Maximum pitch angle in radians.
    };

    /// @brief When false, all mouse input is ignored.
    bool enabled {true};

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

    /**
     * @brief Sets the point the camera orbits around.
     *
     * The camera keeps its current position and re-derives its orbit distance
     * and orientation toward the new target. Any in-flight damped motion is
     * cancelled.
     *
     * @param target World space position to orbit around.
     */
    auto SetTarget(const Vector3& target) -> void;

    ~OrbitControls() override;

private:
    /// @cond INTERNAL
    struct Impl;
    std::unique_ptr<Impl> impl_;
    /// @endcond
};

}