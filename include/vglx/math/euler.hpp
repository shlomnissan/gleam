/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/math/matrix4.hpp"
#include "vglx/math/utilities.hpp"

namespace vglx {

/**
 * @brief Represents 3D Euler angles with pitch, yaw, and roll components.
 *
 * Euler stores orientation using intrinsic Tait–Bryan angles in YXZ order:
 * yaw around the Y-axis is applied first, then pitch around the rotated
 * X-axis, then roll around the rotated Z-axis (equivalent to the three.js
 * Euler order "YXZ"). All angles are specified in radians.
 *
 * Instances can be constructed from individual angles or extracted from a
 * transformation matrix, and converted back to a @ref Matrix4 for use in
 * transforms. Gimbal lock can occur when pitch approaches $\pm 90^{\circ}$, which limits
 * the reliability of angle reconstruction from matrices.
 *
 * @ingroup MathGroup
 */
class Euler  {
public:
    float pitch {0.0f}; ///< Rotation around the X-axis in radians.
    float yaw {0.0f}; ///< Rotation around the Y-axis in radians.
    float roll {0.0f}; ///< Rotation around the Z-axis in radians.

    /**
     * @brief Constructs an Euler object with all angles set to zero.
     */
    constexpr Euler() = default;

    /**
     * @brief Constructs an Euler object from pitch, yaw, and roll.
     *
     * @param pitch Rotation around the X-axis in radians.
     * @param yaw Rotation around the Y-axis in radians.
     * @param roll Rotation around the Z-axis in radians.
     */
    constexpr Euler(float pitch, float yaw, float roll) :
        pitch(pitch),
        yaw(yaw),
        roll(roll) {}

    /**
     * @brief Constructs an Euler object from a transformation matrix.
     *
     * Extracts pitch, yaw, and roll from the given matrix using the YXZ
     * rotation order. When cos(pitch) is close to zero (gimbal lock),
     * roll is set to zero and yaw is computed using an alternate path.
     *
     * @param m Input transformation matrix.
     */
    explicit constexpr Euler(const Matrix4& m) {
        pitch = math::Asin(-m[2].y);
        if (math::Cos(pitch) > 1e-6) {
            yaw = math::Atan2(m[2].x, m[2].z);
            roll = math::Atan2(m[0].y, m[1].y);
        } else {
            // If cos(pitch) is close to zero, we have a gimbal lock
            // and only one of the angles can be determined
            yaw = math::Atan2(-m[0].z, m[0].x);
            roll = 0.0f;
        }
    }

    /**
     * @brief Converts the Euler angles into a 4×4 transformation matrix.
     *
     * The resulting matrix is constructed using the YXZ rotation order.
     */
    [[nodiscard]] constexpr auto GetMatrix() const -> Matrix4 {
        const auto cos_p = math::Cos(pitch);
        const auto sin_p = math::Sin(pitch);
        const auto cos_y = math::Cos(yaw);
        const auto sin_y = math::Sin(yaw);
        const auto cos_r = math::Cos(roll);
        const auto sin_r = math::Sin(roll);

        return Matrix4 {
            cos_y * cos_r + sin_y * sin_p * sin_r, sin_y * sin_p * cos_r - cos_y * sin_r, sin_y * cos_p, 0.0f,
            cos_p * sin_r, cos_p * cos_r, -sin_p, 0.0f,
            cos_y * sin_p * sin_r - sin_y * cos_r, sin_y * sin_r + cos_y * sin_p * cos_r, cos_y * cos_p, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    /**
     * @brief Checks whether all angles are zero.
     */
    [[nodiscard]] constexpr auto IsEmpty() const -> bool {
        return pitch == 0.0f && yaw == 0.0f && roll == 0.0f;
    }

    /**
     * @brief Compares two Euler objects for equality.
     */
    constexpr auto operator==(const Euler&) const -> bool = default;
};

}
