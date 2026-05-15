/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/math/matrix4.hpp"
#include "vglx/math/vector3.hpp"

#include <array>
#include <limits>

namespace vglx {

/**
 * @brief Axis-aligned bounding box defined by minimum and maximum corners.
 *
 * Box3 represents a 3D region bounded by two corners: @ref min and @ref max.
 * It is used for spatial queries, culling tests, intersection checks, and
 * computing bounding volumes. All operations assume the box is axis-aligned
 * in the current coordinate space.
 *
 * @note This class is `constexpr` where possible allowing Box3 instances to be
 * constructed and manipulated at compile time.
 *
 * @ingroup MathGroup
 */
struct Box3 {
    /// @brief Minimum corner of the box.
    Vector3 min {std::numeric_limits<float>::max()};

    /// @brief Maximum corner of the box.
    Vector3 max {std::numeric_limits<float>::lowest()};

    /**
     * @brief Constructs an empty box.
     *
     * The box initializes to an invalid state. Expanding it with points or
     * merging with other boxes produces valid bounds.
     */
    constexpr Box3() = default;

    /**
     * @brief Constructs a box from minimum and maximum corners.
     *
     * @param v_min Minimum point.
     * @param v_max Maximum point.
     */
    constexpr Box3(const Vector3& v_min, const Vector3& v_max) :
        min(v_min),
        max(v_max) {}

    /**
     * @brief Returns the geometric center of the box.
     */
    [[nodiscard]] constexpr auto Center() const -> Vector3 {
        return (min + max) * 0.5f;
    }

    /**
     * @brief Checks whether the box is empty.
     *
     * A box is empty when any component of @ref min exceeds the corresponding
     * component of @ref max, meaning it encloses no volume.
     */
    [[nodiscard]] constexpr auto IsEmpty() const -> bool {
        return min.x > max.x || min.y > max.y || min.z > max.z;
    }

    /**
     * @brief Expands the box to include a point.
     *
     * If the point lies outside the current bounds, @ref min and @ref max are
     * adjusted to enclose it.
     *
     * @param point Point to include.
     */
    constexpr auto ExpandWithPoint(const Vector3& point) -> void {
        if (point.x < min.x) min.x = point.x;
        if (point.y < min.y) min.y = point.y;
        if (point.z < min.z) min.z = point.z;
        if (point.x > max.x) max.x = point.x;
        if (point.y > max.y) max.y = point.y;
        if (point.z > max.z) max.z = point.z;
    }

    /**
     * @brief Resets the box to an empty state.
     *
     * After calling this, the next expanded point will define the new bounds.
     */
    constexpr auto Reset() -> void {
        min = Vector3(std::numeric_limits<float>::max());
        max = Vector3(std::numeric_limits<float>::lowest());
    }

    /**
     * @brief Applies a transform to the box.
     *
     * Computes the axis-aligned bounding box that encloses the transformed
     * eight corners of the original box.
     *
     * @param transform Transformation matrix to apply.
     */
    constexpr auto ApplyTransform(const Matrix4& transform) -> void {
        std::array<Vector3, 8> points_ {};

        points_[0] = transform * Vector3 {min.x, min.y, min.z};
        points_[1] = transform * Vector3 {min.x, min.y, max.z};
        points_[2] = transform * Vector3 {min.x, max.y, min.z};
        points_[3] = transform * Vector3 {min.x, max.y, max.z};
        points_[4] = transform * Vector3 {max.x, min.y, min.z};
        points_[5] = transform * Vector3 {max.x, min.y, max.z};
        points_[6] = transform * Vector3 {max.x, max.y, min.z};
        points_[7] = transform * Vector3 {max.x, max.y, max.z};

        Reset();

        for (const auto& point : points_) ExpandWithPoint(point);
    }

    /**
     * @brief Translates the box.
     *
     * Adds the translation vector to both @ref min and @ref max.
     *
     * @param translation Offset to apply.
     */
    constexpr auto Translate(const Vector3& translation) -> void {
        min += translation;
        max += translation;
    }

    /**
     * @brief Expands this box to contain another box.
     *
     * @param other Box to merge.
     */
    constexpr auto Union(const Box3& other) -> void {
        min.Min(other.min);
        max.Max(other.max);
    }
};

}