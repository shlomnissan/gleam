/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/helpers/orbit_controls.hpp"

#include "vglx/math/matrix4.hpp"
#include "vglx/math/spherical.hpp"
#include "vglx/math/vector2.hpp"
#include "vglx/math/vector3.hpp"

#include "utilities/logger.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace vglx {

namespace {

constexpr float kThetaLimit = vglx::math::pi_over_2 - 0.001f;

auto visible_world_height_at_distance(const Matrix4& projection, float distance) -> float {
    const auto is_ortho = projection[3][3] == 1.0f;
    return 2.0f * (is_ortho ? 1.0f : distance) / projection[1][1];
}

}

struct OrbitControls::Impl {
    Camera* camera;
    OrbitControls::Parameters params;

    Spherical spherical;
    Spherical spherical_delta;

    Vector3 target {Vector3::Zero()};
    Vector3 pan_delta = {Vector3::Zero()};

    Vector2 curr_pos {Vector2::Zero()};
    Vector2 prev_pos {Vector2::Zero()};

    MouseButton curr_button = {MouseButton::None};

    float damping_factor = 0.0f;
    float zoom_scale = 1.0f;

    float min_distance = 0.1f;
    float max_distance = std::numeric_limits<float>::infinity();
    float min_pitch = -kThetaLimit;
    float max_pitch = kThetaLimit;

    Impl(Camera* camera, const Parameters& params) :
      camera(camera),
      params(params) {
        spherical = Spherical {params.radius, params.yaw, params.pitch};
        spherical_delta = Spherical {0.0f, 0.0f, 0.0f};
        target = params.target;
        damping_factor = std::clamp(params.damping_factor, 0.0f, 1.0f);
        min_distance = std::max(params.min_distance, 0.001f);
        max_distance = std::max(params.max_distance, min_distance);
        min_pitch = std::clamp(params.min_pitch, -kThetaLimit, kThetaLimit);
        max_pitch = std::clamp(params.max_pitch, min_pitch, kThetaLimit);
    }

    auto SetTarget(const Vector3& new_target) -> void {
        target = new_target;
        spherical = Spherical::FromVector3(camera->transform.position - target);
        spherical_delta = Spherical {0.0f, 0.0f, 0.0f};
        pan_delta = Vector3::Zero();
        zoom_scale = 1.0f;
    }

    auto OnMouseEvent(MouseEvent* event) {
        using enum MouseEvent::Type;
        using enum MouseButton;

        if (event->window_size.y <= 0.0f) {
            Logger::LogOnce(
                LogLevel::Warning,
                "Cursor motion can't be normalized without a valid window "
                "size. Ignoring malformed mouse input"
            );
            return;
        }

        const auto shift_mod = !!(event->mods & MouseMod::Shift);

        const auto uses_button = [](MouseButton button) {
            return button == Left || button == Right;
        };

        if (event->type == ButtonPressed &&
            curr_button == MouseButton::None &&
            uses_button(event->button)) {
            curr_button = event->button;
            prev_pos = event->position;
            event->handled = true;
        }

        if (event->type == ButtonReleased && curr_button == event->button) {
            curr_button = MouseButton::None;
            event->handled = true;
        }

        if (event->type == Moved) {
            curr_pos = event->position;
            const auto offset = (curr_pos - prev_pos) / event->window_size.y;

            if (curr_button == Left && !shift_mod) {
                spherical_delta.phi -= offset.x * params.orbit_speed;
                spherical_delta.theta += offset.y * params.orbit_speed;
                event->handled = true;
            }

            if (curr_button == Right || (curr_button == Left && shift_mod)) {
                const auto world_height = visible_world_height_at_distance(
                    camera->projection_matrix,
                    spherical.radius
                );

                const auto speed = params.pan_speed * world_height;
                const auto right = camera->Right();
                const auto up = camera->Up();
                pan_delta -= (right * offset.x - up * offset.y) * speed;
                event->handled = true;
            }

            prev_pos = curr_pos;
        }

        if (event->type == Scrolled) {
            zoom_scale *= std::pow(0.95f, event->scroll.y * params.zoom_speed);
            event->handled = true;
        }
    }

    auto OnUpdate(float delta) {
        const auto t = damping_factor > 0.0f
            ? std::pow(1.0f - damping_factor, delta * 60.0f)
            : 0.0f;

        const auto applied = 1.0f - t;
        spherical.phi += spherical_delta.phi * applied;
        spherical.theta += spherical_delta.theta * applied;
        spherical.radius *= std::pow(zoom_scale, applied);
        target += pan_delta * applied;

        spherical_delta.phi *= t;
        spherical_delta.theta *= t;
        zoom_scale = std::pow(zoom_scale, t);
        pan_delta *= t;

        spherical.theta = math::Clamp(spherical.theta, min_pitch, max_pitch);
        spherical.radius = math::Clamp(spherical.radius, min_distance, max_distance);

        camera->transform.SetPosition(target + spherical.ToVector3());
        camera->LookAt(target);
    }
};

OrbitControls::OrbitControls(Camera* camera, const Parameters& params)
    : impl_(std::make_unique<Impl>(camera, params)) {}

auto OrbitControls::OnMouseEvent(MouseEvent* event) -> void {
    if (!enabled) {
        // Drop any active drag so it doesn't resume with stale state
        // when the controls are re-enabled.
        impl_->curr_button = MouseButton::None;
        return;
    }
    impl_->OnMouseEvent(event);
}

auto OrbitControls::OnUpdate(float delta) -> void {
    impl_->OnUpdate(delta);
}

auto OrbitControls::SetTarget(const Vector3& target) -> void {
    impl_->SetTarget(target);
}

OrbitControls::~OrbitControls() = default;

}
