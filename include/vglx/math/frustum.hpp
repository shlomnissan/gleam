/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/math/box3.hpp"
#include "vglx/math/matrix4.hpp"
#include "vglx/math/sphere.hpp"
#include "vglx/math/plane.hpp"

#include <algorithm>
#include <array>

namespace vglx {

/**
 * @brief Represents a view frustum defined by six clipping planes.
 *
 * Frustum encodes the camera’s visible volume using six inward-facing planes:
 * left, right, top, bottom, near, and far. It is typically constructed from a
 * combined view–projection matrix and used by the renderer for view frustum
 * culling.
 *
 * The class provides containment and intersection tests against points,
 * axis-aligned bounding boxes, and bounding spheres. All checks assume that
 * plane normals point into the frustum interior.
 *
 * @ingroup MathGroup
 */
class Frustum {
public:
    /**
     * @brief Constructs an uninitialized frustum.
     */
    constexpr Frustum() = default;

    /**
     * @brief Constructs a frustum from a view–projection matrix.
     *
     * @param view_proj View–projection matrix.
     */
    explicit constexpr Frustum(const Matrix4& view_proj) {
        SetWithViewProjection(view_proj);
    }

    /**
     * @brief Updates the frustum from a view–projection matrix.
     *
     * Extracts the six planes from the matrix and normalizes them.
     *
     * @param view_proj View–projection matrix.
     */
    constexpr auto SetWithViewProjection(const Matrix4& view_proj) -> void {
        planes_[0] = Plane {{
            view_proj(3, 0) + view_proj(0, 0),
            view_proj(3, 1) + view_proj(0, 1),
            view_proj(3, 2) + view_proj(0, 2)
        }, view_proj(3, 3) + view_proj(0, 3)};

        planes_[1] = Plane {{
            view_proj(3, 0) - view_proj(0, 0),
            view_proj(3, 1) - view_proj(0, 1),
            view_proj(3, 2) - view_proj(0, 2)
        }, view_proj(3, 3) - view_proj(0, 3)};

        planes_[2] = Plane {{
            view_proj(3, 0) + view_proj(1, 0),
            view_proj(3, 1) + view_proj(1, 1),
            view_proj(3, 2) + view_proj(1, 2)
        }, view_proj(3, 3) + view_proj(1, 3)};

        planes_[3] = Plane {{
            view_proj(3, 0) - view_proj(1, 0),
            view_proj(3, 1) - view_proj(1, 1),
            view_proj(3, 2) - view_proj(1, 2)
        }, view_proj(3, 3) - view_proj(1, 3)};

        planes_[4] = Plane {{
            view_proj(3, 0) + view_proj(2, 0),
            view_proj(3, 1) + view_proj(2, 1),
            view_proj(3, 2) + view_proj(2, 2)
        }, view_proj(3, 3) + view_proj(2, 3)};

        planes_[5] = Plane {{
            view_proj(3, 0) - view_proj(2, 0),
            view_proj(3, 1) - view_proj(2, 1),
            view_proj(3, 2) - view_proj(2, 2)
        }, view_proj(3, 3) - view_proj(2, 3)};

        for (auto& p : planes_) p.Normalize();
    }

    /**
     * @brief Checks whether a point lies inside the frustum.
     *
     * @param point World-space position to test.
     */
    [[nodiscard]] constexpr auto ContainsPoint(const Vector3& point) const -> bool {
        return std::ranges::all_of(planes_, [&](const auto& plane) {
            return plane.DistanceToPoint(point) >= 0;
        });
    }

    /**
     * @brief Checks whether an axis-aligned bounding box intersects the frustum.
     *
     * Uses a fast AABB–frustum test based on plane normals to reject boxes that
     * lie completely outside the view volume.
     *
     * @param box Box to test.
     */
    [[nodiscard]] constexpr auto IntersectsWithBox3(const Box3& box) const -> bool {
        auto v = Vector3::Zero();
        return std::ranges::all_of(planes_, [&](const auto& plane) {
            v.x = plane.normal.x > 0 ? box.max.x : box.min.x;
            v.y = plane.normal.y > 0 ? box.max.y : box.min.y;
            v.z = plane.normal.z > 0 ? box.max.z : box.min.z;
            return plane.DistanceToPoint(v) >= 0;
        });
    }

    /**
     * @brief Checks whether a sphere intersects the frustum.
     *
     * @param sphere Bounding sphere to test.
     */
    [[nodiscard]] constexpr auto IntersectsWithSphere(const Sphere& sphere) const -> bool {
        return std::ranges::all_of(planes_, [&](const auto& plane) {
            const auto distance = plane.DistanceToPoint(sphere.center);
            return distance >= -sphere.radius;
        });
    }

private:
    std::array<Plane, 6> planes_ = {};
};

}