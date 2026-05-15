/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/math/matrix4.hpp"
#include "vglx/math/utilities.hpp"
#include "vglx/math/vector3.hpp"

#include <algorithm>

namespace vglx {

/**
 * @brief Represents a bounding sphere in 3D space.
 *
 * Sphere stores a center point and a radius and is used as a simple bounding
 * volume for intersection tests, culling, and spatial queries. A negative
 * radius indicates an empty sphere. The class supports expansion with points,
 * merging with other spheres, translation, and transformation by a matrix.
 *
 * @ingroup MathGroup
 */
struct Sphere {
    /// @brief Sphere center in space.
    Vector3 center {Vector3::Zero()};

    /// @brief Sphere radius. A negative value indicates an empty sphere.
    float radius {-1.0f};

    /**
     * @brief Constructs an empty sphere.
     */
    constexpr Sphere() = default;

    /**
     * @brief Constructs a sphere from a center and radius.
     *
     * @param center Sphere center.
     * @param radius Sphere radius (negative indicates empty).
     */
    constexpr Sphere(const Vector3 center, float radius) :
        center(center),
        radius(radius) {}

    /**
     * @brief Returns the sphere radius.
     */
    [[nodiscard]] constexpr auto Radius() const -> float { return radius; }

    /**
     * @brief Resets the sphere to an empty state.
     */
    constexpr auto Reset() -> void {
        center = Vector3::Zero();
        radius = -1.0f;
    }

    /**
     * @brief Checks whether the sphere is empty.
     */
    [[nodiscard]] constexpr auto IsEmpty() const -> bool {
        return radius < 0.0f;
    }

    /**
     * @brief Expands the sphere to include a point.
     *
     * If the sphere is empty, it becomes a zero-radius sphere at the point.
     * Otherwise, the sphere grows minimally to contain the point.
     *
     * @param p Point to include.
     */
    constexpr auto ExpandWithPoint(const Vector3& p) -> void {
        if (IsEmpty()) {
            center = p;
            radius = 0.0f;
            return;
        }

        const auto delta = p - center;
        const auto length_sqr = delta.LengthSquared();
        if (length_sqr > radius * radius) {
            const auto length = math::Sqrt(length_sqr);
            const auto half_way = (length - radius) * 0.5f;
            center += delta * (half_way / length);
            radius += half_way;
        }
    }

    /**
     * @brief Applies a 4×4 transform to the sphere.
     *
     * The center is transformed directly, while the radius is scaled by the
     * largest scale factor present in the matrix to ensure conservative
     * bounding behavior.
     *
     * @param transform Matrix to apply.
     */
    constexpr auto ApplyTransform(const Matrix4& transform) -> void {
        center = transform * center;
        auto& t0 = transform[0];
        auto& t1 = transform[1];
        auto& t2 = transform[2];

        radius *= math::Sqrt(std::max({
            Vector3 {t0.x, t0.y, t0.z}.LengthSquared(),
            Vector3 {t1.x, t1.y, t1.z}.LengthSquared(),
            Vector3 {t2.x, t2.y, t2.z}.LengthSquared(),
        }));
    }

    /**
     * @brief Translates the sphere by a vector.
     *
     * @param translation Offset to apply.
     */
    constexpr auto Translate(const Vector3& translation) -> void {
        center += translation;
    }

    /**
     * @brief Expands this sphere to fully contain another sphere.
     *
     * Handles empty spheres, identical centers, and general cases using two
     * directional expansion points.
     *
     * @param other Sphere to merge.
     */
    constexpr auto Union(const Sphere& other) -> void {
        if (other.IsEmpty()) return;

        if (IsEmpty()) {
            center = other.center;
            radius = other.radius;
            return;
        }

        if (center == other.center) {
            radius = std::max(radius, other.radius);
            return;
        }

        auto radius_dir = Normalize(other.center - center) * other.radius;
        ExpandWithPoint(other.center + radius_dir);
        ExpandWithPoint(other.center - radius_dir);
    }
};

}