/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/nodes/mesh.hpp"
#include "vglx/math/color.hpp"
#include "vglx/math/matrix4.hpp"

#include <memory>
#include <optional>
#include <vector>

namespace vglx {

/**
 * @brief A mesh node that renders many instances of the same geometry efficiently.
 *
 * `InstancedMesh` draws multiple copies of a single geometry-material pair using
 * per‑instance transforms and optional per‑instance colors. This is ideal for large
 * numbers of similar objects (e.g., vegetation, crowds, particles) with different
 * positions or tints while keeping draw calls low.
 *
 * Each instance can have:
 * - a transform matrix (model matrix)
 * - a color (for material tinting)
 *
 * Instances are addressed by zero‑based index in the range `[0, Count())`.
 *
 * @code
 * const auto geometry = BoxGeometry::Create({1.0f, 1.0f, 1.0f});
 * const auto material = PhongMaterial::Create(0xFFFFFF);
 *
 * auto boxes = vglx::InstancedMesh::Create(geometry, material, 2500);
 *
 * for (auto i = 0; i < 50; ++i) {
 *      for (auto j = 0; j < 50; ++j) {
 *          auto t = Transform3 {};
 *          t.SetPosition({i * 2.0f - 49.0f, j * 2.0f - 49.0f, 0.0f});
 *          boxes->SetTransformAt(j * 50 + i, t);
 *      }
 * }
 *
 * my_scene->Add(boxes);
 * @endcode
 *
 * @note
 * - Out-of-range indices are invalid and result in undefined behavior.
 * - Frustum culling is performed once per draw call using a single
 *   bounding sphere that encloses all instances (cluster-level culling).
 *
 * @ingroup NodesGroup
 */
class VGLX_EXPORT InstancedMesh : public Mesh {
public:
    /**
     * @brief Constructs an instanced mesh.
     *
     * @param geometry Shared pointer to a geometry for all instances.
     * @param material Shared pointer to a material for all instances.
     * @param count Number of instances to allocate.
     */
    InstancedMesh(
        std::shared_ptr<Geometry> geometry,
        std::shared_ptr<Material> material,
        std::size_t count
    );

    /**
     * @brief Creates a shared pointer to an InstancedMesh object.
     *
     * @param geometry Shared pointer to a geometry for all instances.
     * @param material Shared pointer to a material for all instances.
     * @param count Number of instances to allocate.
     * @return std::shared_ptr<InstancedMesh>
     */
    [[nodiscard]] static auto Create(
        std::shared_ptr<Geometry> geometry,
        std::shared_ptr<Material> material,
        std::size_t count
    ) {
        return std::make_shared<InstancedMesh>(geometry, material, count);
    }

    /**
     * @brief Returns node type.
     *
     * @return Node::Type::InstancedMesh
     */
    [[nodiscard]] auto GetNodeType() const -> Node::Type override {
        return Node::Type::InstancedMesh;
    }

    /**
     * @brief Returns the number of instances in this mesh.
     *
     * @return Instance count.
     */
    [[nodiscard]] auto Count() { return count_; }

    /**
     * @brief Returns the color assigned to a specific instance.
     *
     * @param idx Instance index in [0, Count()).
     * @return Color of the instance.
     */
    [[nodiscard]] auto GetColorAt(std::size_t idx) -> const Color;

    /**
     * @brief Returns the transform assigned to a specific instance.
     *
     * @param idx Instance index in [0, Count()).
     * @return Model matrix of the instance.
     */
    [[nodiscard]] auto GetTransformAt(std::size_t idx) -> const Matrix4;

    /**
     * @brief Sets the color for a specific instance.
     *
     * This color typically multiplies or tints the base material color.
     *
     * @param idx Instance index in [0, Count()).
     * @param color Color to assign.
     */
    auto SetColorAt(std::size_t idx, const Color& color) -> void;

    /**
     * @brief Sets the model transform for a specific instance.
     *
     * @param idx Instance index in [0, Count()).
     * @param matrix Transform matrix to assign.
     */
    auto SetTransformAt(std::size_t idx, const Matrix4& matrix) -> void;

    /**
     * @brief Sets the model transform for a specific instance from a Transform3.
     *
     * Convenience overload that extracts the matrix from a `Transform3`.
     *
     * @param idx Instance index in [0, Count()).
     * @param transform Transform providing the model matrix.
     */
    auto SetTransformAt(std::size_t idx, Transform3& transform) -> void;

    /**
     * @brief Returns the instanced mesh cluster bounding box.
     */
    auto BoundingBox() -> Box3 override;

    /**
     * @brief Returns the instanced mesh cluster bounding sphere.
     */
    auto BoundingSphere() -> Sphere override;

    /**
     * @brief Destructor.
     */
    ~InstancedMesh();

private:
    /// @brief Per-instance colors indexed by instance ID.
    std::vector<Color> colors_;

    /// @cond INTERNAL
    friend class GLBuffers;
    class Impl;
    std::unique_ptr<Impl> impl_;
    /// @endcond

     /// @brief Per-instance model matrices indexed by instance ID.
    std::vector<Matrix4> transforms_;

    /// @brief Cached bounding box.
    std::optional<Box3> bounding_box_;

    /// @brief Cached bounding sphere.
    std::optional<Sphere> bounding_sphere_;

    /// @brief Number of instances.
    std::size_t count_;
};

}