/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/math/vector3.hpp"
#include "vglx/math/utilities.hpp"

constexpr float thetha_limit = vglx::math::pi_over_2 - 0.001f;

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
struct VGLX_EXPORT Spherical {
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
     * @brief Clamps the polar angle to avoid degeneracy at the poles.
     *
     * Ensures @ref theta stays within a safe range so azimuth calculations
     * remain well-defined. If @ref phi drifts outside $[-2π, 2π]$ it is
     * wrapped back into range.
     */
    constexpr auto MakeSafe() -> void {
        theta = math::Clamp(theta, -thetha_limit, thetha_limit);
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