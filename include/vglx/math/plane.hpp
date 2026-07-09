/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/math/sphere.hpp"
#include "vglx/math/utilities.hpp"
#include "vglx/math/vector3.hpp"

namespace vglx {

/**
 * @brief Represents a plane in 3D space.
 *
 * Plane is stored in the implicit form $n · x + d = 0$, where @ref normal is
 * the plane normal and @ref distance is the signed distance from the origin
 * along that normal. It is commonly used for half-space tests, frustum planes,
 * and simple clipping or culling operations.
 *
 * @ingroup MathGroup
 */
struct Plane {
    /// @brief Plane normal vector.
    Vector3 normal {Vector3::Y()};

    /// @brief Signed distance from the origin to the plane along the normal.
    float distance {0.0f};

    /**
     * @brief Constructs a default plane with an upward normal.
     */
    constexpr Plane() = default;

    /**
     * @brief Constructs a plane from a normal and distance.
     *
     * @param normal Plane normal.
     * @param distance Signed distance from the origin along the normal.
     */
    constexpr Plane(const Vector3& normal, float distance) :
        normal(normal),
        distance(distance) {}

    /**
     * @brief Computes the signed distance from the plane to a point.
     *
     * Positive values are on the side pointed to by the normal, negative
     * values are behind the plane, and zero lies on the plane.
     *
     * @param point Point to test.
     */
    [[nodiscard]] constexpr auto DistanceToPoint(const Vector3& point) const -> float {
        return Dot(normal, point) + distance;
    }

    /**
     * @brief Computes the signed distance from the plane to a sphere surface.
     *
     * The value is the distance from the plane to the closest point on the
     * sphere. A negative value means the sphere penetrates the plane.
     *
     * @param sphere Sphere to test.
     */
    [[nodiscard]] constexpr auto DistanceToSphere(const Sphere& sphere) const -> float {
        return DistanceToPoint(sphere.center) - sphere.radius;
    }

    /**
     * @brief Normalizes the plane.
     *
     * Scales @ref normal to unit length and adjusts @ref distance to preserve
     * the plane equation.
     */
    constexpr auto Normalize() -> void {
        const auto inverse_length = math::InverseSqrt(Dot(normal, normal));
        normal *= inverse_length;
        distance *= inverse_length;
    }
};

}