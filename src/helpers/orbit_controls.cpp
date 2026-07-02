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

    Impl(Camera* camera, const Parameters& params) :
      camera(camera),
      params(params) {
        spherical = Spherical {params.radius, params.yaw, params.pitch};
        spherical_delta = Spherical {0.0f, 0.0f, 0.0f};
        damping_factor = std::clamp(params.damping_factor, 0.0f, 1.0f);
    }

    auto OnMouseEvent(MouseEvent* event) {
        using enum MouseEvent::Type;
        using enum MouseButton;

        if (camera->ViewportHeight() <= 0) {
            Logger::Log(
                LogLevel::Error,
                "OrbitControls requires a sized camera but Camera::Resize "
                "was never called; ignoring mouse input"
            );
            return;
        }

        const auto shift_mod = !!(event->mods & MouseMod::Shift);

        if (event->type == ButtonPressed && curr_button == MouseButton::None) {
            curr_button = event->button;
            prev_pos = event->position;
        }

        if (event->type == ButtonReleased && curr_button == event->button) {
            curr_button = MouseButton::None;
        }

        if (event->type == Moved) {
            curr_pos = event->position;
            const auto offset =
                (curr_pos - prev_pos) / static_cast<float>(camera->ViewportHeight());

            if (curr_button == Left && !shift_mod) {
                spherical_delta.phi -= offset.x * params.orbit_speed;
                spherical_delta.theta += offset.y * params.orbit_speed;
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
            }

            prev_pos = curr_pos;
        }

        if (event->type == Scrolled) {
            zoom_scale *= std::pow(0.95f, event->scroll.y * params.zoom_speed);
        }
    }

    auto OnUpdate(float delta) {
        spherical.phi += spherical_delta.phi;
        spherical.theta += spherical_delta.theta;
        spherical.radius *= zoom_scale;
        target += pan_delta;

        spherical.theta = math::Clamp(spherical.theta, -kThetaLimit, kThetaLimit);
        spherical.radius = std::max(0.1f, spherical.radius);

        camera->transform.SetPosition(target + spherical.ToVector3());
        camera->LookAt(target);

        if (damping_factor > 0.0f) {
            // Framerate-independent damping. The exponent normalizes
            // to 60 fps so the damping_factor parameter keeps its
            // original per-frame meaning at that reference rate.
            const auto t = std::pow(1.0f - damping_factor, delta * 60.0f);
            spherical_delta.phi *= t;
            spherical_delta.theta *= t;
            zoom_scale = math::Lerp(1.0f, zoom_scale, t);
            pan_delta *= t;
        } else {
            spherical_delta = Spherical {0.0f, 0.0f, 0.0f};
            zoom_scale = 1.0f;
            pan_delta = Vector3::Zero();
        }
    }
};

OrbitControls::OrbitControls(Camera* camera, const Parameters& params)
    : impl_(std::make_unique<Impl>(camera, params)) {}

auto OrbitControls::OnMouseEvent(MouseEvent* event) -> void {
    impl_->OnMouseEvent(event);
}

auto OrbitControls::OnUpdate(float delta) -> void {
    impl_->OnUpdate(delta);
}

OrbitControls::~OrbitControls() = default;

}