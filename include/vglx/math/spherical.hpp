/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/math/vector3.hpp"
#include "vglx/math/utilities.hpp"

namespace vglx {

/**
 * @brief Represents a point in spherical coordinates.
 *
 * Spherical stores a 3D position using `(radius, phi, theta)` where `radius`
 * is the distance from the origin, `phi` is the azimuth angle around the Y-axis,
 * and `theta` is the polar angle measured from the equatorial plane. This
 * representation is commonly used for orbital camera rigs, direction sampling,
 * and converting between angular and Cartesian representations.
 *
 * @ingroup MathGroup
 */
struct Spherical {
    /// @brief Radial distance from the origin.
    float radius = 1.0f;

    /// @brief Azimuth angle around the Y-axis in radians.
    float phi = 0.0f;

    /// @brief Polar angle from the equator in radians.
    float theta = 0.0f;

    /**
     * @brief Constructs a spherical coordinate at the origin direction.
     */
    constexpr Spherical() = default;

    /**
     * @brief Constructs a spherical coordinate from radius, phi, and theta.
     *
     * @param radius Radial distance from the origin.
     * @param phi Azimuth angle in radians.
     * @param theta Polar angle in radians.
     */
    constexpr Spherical(float radius, float phi, float theta)
        : radius(radius), phi(phi), theta(theta) {}

    /**
     * @brief Creates a spherical coordinate from a @ref Vector3.
     *
     * This is the inverse of @ref ToVector3 using the same convention.
     * A zero-length vector produces a default spherical coordinate.
     *
     * @param v Cartesian position to convert.
     */
    [[nodiscard]] static constexpr auto FromVector3(const Vector3& v) -> Spherical {
        const auto radius = v.Length();
        if (radius == 0.0f) return {};
        return {radius, math::Atan2(v.x, v.z), math::Asin(v.y / radius)};
    }

    /**
     * @brief Converts this spherical coordinate to a @ref Vector3.
     *
     * In this convention, `phi` equals 0 along the +Z axis and increases toward +X.
     * The polar angle `theta` is 0 on the equator, reaches $\frac{\pi}{2}$ at +Y,
     * and $-\frac{\pi}{2}$ at -Y.
     */
    [[nodiscard]] constexpr auto ToVector3() const -> Vector3 {
        const auto c = math::Cos(theta);
        return Vector3 {
            radius * math::Sin(phi) * c,
            radius * math::Sin(theta),
            radius * math::Cos(phi) * c
        };
    }

    /**
     * @brief Compares two spherical objects for equality.
     */
    constexpr auto operator==(const Spherical&) const -> bool = default;
};

/**
 * @brief Linearly interpolates between two spherical coordinates.
 * @related Spherical
 *
 * @param a Start color.
 * @param b End color.
 * @param f Interpolation factor in $[0, 1]$.
 */
[[nodiscard]] constexpr auto Lerp(const Spherical& a, const Spherical& b, float f) -> Spherical {
    return Spherical {
        math::Lerp(a.radius, b.radius, f),
        math::Lerp(a.phi, b.phi, f),
        math::Lerp(a.theta, b.theta, f),
    };
}

}