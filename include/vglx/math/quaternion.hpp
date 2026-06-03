/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/math/matrix4.hpp"
#include "vglx/math/vector3.hpp"
#include "vglx/math/utilities.hpp"

namespace vglx {

struct Quaternion;
auto constexpr Dot(const Quaternion& a, const Quaternion& b) -> float;

/**
 * @brief Represents a rotation as a unit quaternion.
 *
 * Quaternion stores four components `(x, y, z, w)`, where `(x, y, z)` is the
 * vector part and `w` is the scalar part. Unit quaternions encode 3D rotations
 * without the gimbal lock and order ambiguity of Euler angles, compose through
 * multiplication, and interpolate smoothly.
 *
 * Rotations assume the quaternion is normalized. Construct one from an axis and
 * angle with @ref FromAxisAngle, extract a @ref Matrix4 with @ref GetMatrix, or
 * recover a quaternion from a rotation matrix using the matrix constructor.
 *
 * @ingroup MathGroup
 */
struct Quaternion {
    /// @brief X component of the vector part.
    float x {0.0f};
    /// @brief Y component of the vector part.
    float y {0.0f};
    /// @brief Z component of the vector part.
    float z {0.0f};
    /// @brief Scalar component.
    float w {1.0f};

    /**
     * @brief Constructs the identity quaternion `(0, 0, 0, 1)`.
     */
    constexpr Quaternion() = default;

    /**
     * @brief Constructs a quaternion from individual components.
     *
     * @param x X component of the vector part.
     * @param y Y component of the vector part.
     * @param z Z component of the vector part.
     * @param w Scalar component.
     */
    constexpr Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    /**
     * @brief Constructs a quaternion from a rotation matrix.
     *
     * Recovers the rotation encoded in the upper-left 3×3 block of the matrix
     * using the branch that maximizes numerical stability. The matrix is
     * assumed to be a pure rotation (orthonormal, no scale or shear).
     *
     * @param m Input rotation matrix.
     */
    explicit constexpr Quaternion(const Matrix4& m) {
        const auto m00 = m(0, 0), m11 = m(1, 1), m22 = m(2, 2);
        const auto trace = m00 + m11 + m22;

        if (trace > 0.0f) {
            const auto s = math::Sqrt(trace + 1.0f) * 2.0f;
            w = 0.25f * s;
            x = (m(2, 1) - m(1, 2)) / s;
            y = (m(0, 2) - m(2, 0)) / s;
            z = (m(1, 0) - m(0, 1)) / s;
        } else if (m00 > m11 && m00 > m22) {
            const auto s = math::Sqrt(1.0f + m00 - m11 - m22) * 2.0f;
            w = (m(2, 1) - m(1, 2)) / s;
            x = 0.25f * s;
            y = (m(0, 1) + m(1, 0)) / s;
            z = (m(0, 2) + m(2, 0)) / s;
        } else if (m11 > m22) {
            const auto s = math::Sqrt(1.0f + m11 - m00 - m22) * 2.0f;
            w = (m(0, 2) - m(2, 0)) / s;
            x = (m(0, 1) + m(1, 0)) / s;
            y = 0.25f * s;
            z = (m(1, 2) + m(2, 1)) / s;
        } else {
            const auto s = math::Sqrt(1.0f + m22 - m00 - m11) * 2.0f;
            w = (m(1, 0) - m(0, 1)) / s;
            x = (m(0, 2) + m(2, 0)) / s;
            y = (m(1, 2) + m(2, 1)) / s;
            z = 0.25f * s;
        }
    }

    /**
     * @brief Returns the identity quaternion `(0, 0, 0, 1)`.
     */
    [[nodiscard]] static constexpr auto Identity() -> Quaternion { return {0.0f, 0.0f, 0.0f, 1.0f}; }

    /**
     * @brief Builds a quaternion from an axis and angle.
     *
     * @param axis Rotation axis; normalized internally.
     * @param angle Rotation angle in radians.
     */
    [[nodiscard]] static constexpr auto FromAxisAngle(const Vector3& axis, float angle) -> Quaternion {
        const auto half = angle * 0.5f;
        const auto s = math::Sin(half);
        const auto n = vglx::Normalize(axis);
        return {n.x * s, n.y * s, n.z * s, math::Cos(half)};
    }

    /**
     * @brief Returns the quaternion length.
     */
    [[nodiscard]] constexpr auto Length() const -> float { return math::Sqrt(Dot(*this, *this)); }

    /**
     * @brief Returns the squared quaternion length.
     *
     * Useful when comparing lengths without paying the cost of a square root.
     */
    [[nodiscard]] constexpr auto LengthSquared() const -> float { return Dot(*this, *this); }

    /**
     * @brief Normalizes the quaternion in-place.
     *
     * If the length is zero, the quaternion is left unchanged.
     */
    constexpr auto Normalize() -> Quaternion& {
        const auto len = Length();
        if (len == 0.0f) return *this;
        const auto inv = 1.0f / len;
        x *= inv;
        y *= inv;
        z *= inv;
        w *= inv;
        return *this;
    }

    /**
     * @brief Converts the quaternion into a 4×4 rotation matrix.
     *
     * Assumes the quaternion is normalized.
     */
    [[nodiscard]] constexpr auto GetMatrix() const -> Matrix4 {
        const auto xx = x * x, yy = y * y, zz = z * z;
        const auto xy = x * y, xz = x * z, yz = y * z;
        const auto wx = w * x, wy = w * y, wz = w * z;

        return Matrix4 {
            1.0f - 2.0f * (yy + zz), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f,
            2.0f * (xy + wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz - wx), 0.0f,
            2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (xx + yy), 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    /**
     * @brief Compares two quaternions for equality.
     */
    constexpr auto operator==(const Quaternion&) const -> bool = default;
};

/**
 * @brief Negates a quaternion.
 * @related Quaternion
 *
 * The result represents the same rotation as the input.
 *
 * @param q Input quaternion.
 */
[[nodiscard]] constexpr auto operator-(const Quaternion& q) -> Quaternion {
    return {-q.x, -q.y, -q.z, -q.w};
}

/**
 * @brief Adds two quaternions component-wise.
 * @related Quaternion
 *
 * @param a First quaternion.
 * @param b Second quaternion.
 */
[[nodiscard]] constexpr auto operator+(const Quaternion& a, const Quaternion& b) -> Quaternion {
    return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

/**
 * @brief Subtracts one quaternion from another component-wise.
 * @related Quaternion
 *
 * @param a First quaternion.
 * @param b Second quaternion.
 */
[[nodiscard]] constexpr auto operator-(const Quaternion& a, const Quaternion& b) -> Quaternion {
    return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

/**
 * @brief Multiplies a quaternion by a scalar.
 * @related Quaternion
 *
 * @param q Input quaternion.
 * @param n Scalar value.
 */
[[nodiscard]] constexpr auto operator*(const Quaternion& q, float n) -> Quaternion {
    return {q.x * n, q.y * n, q.z * n, q.w * n};
}

/**
 * @brief Multiplies a scalar by a quaternion.
 * @related Quaternion
 *
 * @param n Scalar value.
 * @param q Input quaternion.
 */
[[nodiscard]] constexpr auto operator*(float n, const Quaternion& q) -> Quaternion {
    return q * n;
}

/**
 * @brief Composes two rotations using the Hamilton product.
 * @related Quaternion
 *
 * The result applies `b` first, then `a`, mirroring matrix multiplication.
 * Quaternion multiplication is not commutative.
 *
 * @param a Second rotation to apply.
 * @param b First rotation to apply.
 */
[[nodiscard]] constexpr auto operator*(const Quaternion& a, const Quaternion& b) -> Quaternion {
    return {
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
    };
}

/**
 * @brief Rotates a 3D vector by a quaternion.
 * @related Quaternion
 *
 * Assumes the quaternion is normalized.
 *
 * @param q Rotation quaternion.
 * @param v Vector to rotate.
 */
[[nodiscard]] constexpr auto operator*(const Quaternion& q, const Vector3& v) -> Vector3 {
    const auto u = Vector3 {q.x, q.y, q.z};
    const auto t = 2.0f * Cross(u, v);
    return v + q.w * t + Cross(u, t);
}

/**
 * @brief Computes the dot product of two quaternions.
 * @related Quaternion
 *
 * @param a First quaternion.
 * @param b Second quaternion.
 */
[[nodiscard]] constexpr auto Dot(const Quaternion& a, const Quaternion& b) -> float {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

/**
 * @brief Returns the conjugate of a quaternion.
 * @related Quaternion
 *
 * Negates the vector part, leaving the scalar part unchanged. For a unit
 * quaternion the conjugate is also its inverse.
 *
 * @param q Input quaternion.
 */
[[nodiscard]] constexpr auto Conjugate(const Quaternion& q) -> Quaternion {
    return {-q.x, -q.y, -q.z, q.w};
}

/**
 * @brief Returns the inverse of a quaternion.
 * @related Quaternion
 *
 * If the quaternion has zero length, the identity quaternion is returned.
 *
 * @param q Input quaternion.
 */
[[nodiscard]] constexpr auto Inverse(const Quaternion& q) -> Quaternion {
    const auto len_sq = q.LengthSquared();
    return len_sq == 0.0f ? Quaternion::Identity() : Conjugate(q) * (1.0f / len_sq);
}

/**
 * @brief Returns a normalized copy of a quaternion.
 * @related Quaternion
 *
 * If the input has zero length, the identity quaternion is returned.
 *
 * @param q Input quaternion.
 */
[[nodiscard]] constexpr auto Normalize(const Quaternion& q) -> Quaternion {
    const auto len = q.Length();
    return len == 0.0f ? Quaternion::Identity() : q * (1.0f / len);
}

/**
 * @brief Spherically interpolates between two quaternions.
 * @related Quaternion
 *
 * Interpolates along the shortest arc and falls back to normalized linear
 * interpolation when the inputs are nearly parallel.
 *
 * @param a Start rotation.
 * @param b End rotation.
 * @param f Interpolation factor in $[0, 1]$.
 */
[[nodiscard]] constexpr auto Slerp(const Quaternion& a, const Quaternion& b, float f) -> Quaternion {
    auto cos_theta = Dot(a, b);

    // Take the shorter path; q and -q represent the same rotation.
    auto end = b;
    if (cos_theta < 0.0f) {
        end = -b;
        cos_theta = -cos_theta;
    }

    // Nearly parallel: fall back to normalized linear interpolation.
    if (cos_theta > 0.9995f) {
        return Normalize(a + (end - a) * f);
    }

    const auto theta = math::Atan2(math::Sqrt(1.0f - cos_theta * cos_theta), cos_theta);
    const auto sin_theta = math::Sin(theta);
    const auto wa = math::Sin((1.0f - f) * theta) / sin_theta;
    const auto wb = math::Sin(f * theta) / sin_theta;

    return a * wa + end * wb;
}

}
