/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/cameras/perspective_camera.hpp"

#include <cmath>

namespace vglx {

namespace {

auto make_perspective_proj(const PerspectiveCamera::Parameters& params) {
    const auto tan_half_fov = std::tan(params.fov / 2);
    auto mat = Matrix4 {0.0f};
    mat[0] = {1.0f / (params.aspect * tan_half_fov), 0.0f, 0.0f, 0.0f};
    mat[1] = {0.0f, 1.0f / tan_half_fov, 0.0f, 0.0f};
    mat[2] = {0.0f, 0.0f, -(params.far + params.near) / (params.far - params.near), -1.0f};
    mat[3] = {0.0f, 0.0f, -(2 * params.far * params.near) / (params.far - params.near), 0.0f};
    return mat;
}

}

PerspectiveCamera::PerspectiveCamera(const Parameters& params) : params_(params) {
    projection_matrix = make_perspective_proj(params_);
}

auto PerspectiveCamera::OnResize(int width, int height) -> void {
    params_.aspect = static_cast<float>(width) / static_cast<float>(height);
    projection_matrix = make_perspective_proj(params_);
}

auto PerspectiveCamera::SetLens(float fov, float near, float far) -> void {
    params_.fov = fov;
    params_.near = near;
    params_.far = far;
    projection_matrix = make_perspective_proj(params_);
}

}