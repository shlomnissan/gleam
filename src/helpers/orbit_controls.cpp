/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/helpers/orbit_controls.hpp"

#include "vglx/math/spherical.hpp"
#include "vglx/math/vector2.hpp"
#include "vglx/math/vector3.hpp"

#include <cmath>

namespace vglx {

namespace {

constexpr float kThetaLimit = vglx::math::pi_over_2 - 0.001f;

}

struct OrbitControls::Impl {
    Camera* camera;
    SharedContextPointer context = nullptr;
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
            auto offset = curr_pos - prev_pos;

            const auto h = context ? static_cast<float>(context->window_height) : 1.0f;

            if (curr_button == Left && !shift_mod) {
                spherical_delta.phi -= offset.x / h * params.orbit_speed;
                spherical_delta.theta += offset.y / h * params.orbit_speed;
            }

            if (curr_button == Right || (curr_button == Left && shift_mod)) {
                const auto speed = params.pan_speed * spherical.radius / h;
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

auto OrbitControls::OnAttached(SharedContextPointer context) -> void {
    impl_->context = context;
}

auto OrbitControls::OnMouseEvent(MouseEvent* event) -> void {
    impl_->OnMouseEvent(event);
}

auto OrbitControls::OnUpdate(float delta) -> void {
    impl_->OnUpdate(delta);
}

OrbitControls::~OrbitControls() = default;

}