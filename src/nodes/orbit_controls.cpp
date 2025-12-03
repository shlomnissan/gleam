/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/nodes/orbit_controls.hpp"

#include "vglx/math/spherical.hpp"
#include "vglx/math/vector2.hpp"
#include "vglx/math/vector3.hpp"

#include <cmath>

namespace vglx {

namespace {

constexpr float limit = vglx::math::pi_over_2 - 0.001f;

}

struct OrbitControls::Impl {
    Camera* camera;

    Spherical curr_orientation {};
    Spherical target_orientation {};

    Vector3 curr_center = Vector3::Zero();
    Vector3 target_center = Vector3::Zero();

    Vector2 curr_pos {0.0f, 0.0f};
    Vector2 prev_pos {0.0f, 0.0f};

    float damping_factor {0.0f};
    float orbit_speed {0.0f};
    float pan_speed {0.0f};
    float zoom_speed {0.0f};
    float curr_scroll_offset {0.0f};

    MouseButton curr_button {MouseButton::None};

    bool shift_key_pressed {false};

    auto OnUpdate(float delta) {
        using enum MouseButton;

        const auto offset = curr_pos - prev_pos;
        const auto do_orbit = curr_button == Left && !shift_key_pressed;
        const auto do_pan = curr_button == Right || curr_button == Left && shift_key_pressed;
        const auto do_zoom = curr_scroll_offset != 0.0f;

        if (do_orbit) {
            target_orientation.phi -= offset.x * orbit_speed;
            target_orientation.theta += offset.y * orbit_speed;
            target_orientation.theta = math::Clamp(target_orientation.theta, -limit, limit);
        }

        if (do_zoom) {
            target_orientation.radius *= std::pow(zoom_speed, curr_scroll_offset);
            target_orientation.radius = std::max(0.1f, target_orientation.radius);
            curr_scroll_offset = 0.0f;
        }

        if (do_pan) {
            const auto speed = pan_speed * target_orientation.radius;
            const auto right = camera->Right();
            const auto up = camera->Up();
            target_center -= (right * offset.x - up * offset.y) * speed;
        }

        prev_pos = curr_pos;

        const auto t = 1.0f - std::exp(-damping_factor * delta);
        curr_orientation = Lerp(curr_orientation, target_orientation, t);
        curr_center = Lerp(curr_center, target_center, t);

        camera->transform.SetPosition(curr_center + curr_orientation.ToVector3());
        camera->LookAt(curr_center);
    }
};

OrbitControls::OrbitControls(Camera* camera, const Parameters& params)
    : impl_(std::make_unique<Impl>())
{
    impl_->camera = camera;
    impl_->orbit_speed = params.orbit_speed;
    impl_->pan_speed = params.pan_speed;
    impl_->zoom_speed = params.zoom_speed;
    impl_->damping_factor = params.damping_factor;

    const auto s = Spherical {params.radius, params.yaw, params.pitch};
    impl_->target_orientation = s;
    impl_->curr_orientation = s;
};

auto OrbitControls::OnMouseEvent(MouseEvent* event) -> void {
    impl_->curr_pos = event->position;
    impl_->shift_key_pressed = event->mods & MouseMod::Shift;

    const auto is_pressed = event->type == MouseEvent::Type::ButtonPressed;
    if (is_pressed && impl_->curr_button == MouseButton::None) {
        impl_->curr_button = event->button;
    }

    const auto is_released = event->type == MouseEvent::Type::ButtonReleased;
    if (is_released && event->button == impl_->curr_button) {
        impl_->curr_button = MouseButton::None;
    }

    if (event->type == MouseEvent::Type::Scrolled) {
        impl_->curr_scroll_offset = event->scroll.y;
    }
}

auto OrbitControls::OnUpdate(float delta) -> void {
    impl_->OnUpdate(delta);
}

OrbitControls::~OrbitControls() = default;

}