/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/math/color.hpp"
#include "vglx/math/matrix4.hpp"
#include "vglx/scene/mesh.hpp"

#include <memory>
#include <optional>
#include <vector>

namespace vglx {

/**
 * @brief Renderable node that draws many copies of the same mesh efficiently.
 *
 * InstancedMesh stores a single geometry/material pair and renders it multiple
 * times in a single draw call using per-instance transforms and optional colors.
 * This dramatically improves performance when drawing large numbers of identical
 * objects by reducing CPU overhead and state changes.
 *
 * Each instance is addressed by a zero-based index in the range $[0, count)$.
 * Transforms and colors can be queried or updated individually.
 *
 * @code
 * const auto geometry = vglx::BoxGeometry::Create({1.0f, 1.0f, 1.0f});
 * const auto material = vglx::PhongMaterial::Create(0xFFFFFF);
 *
 * auto boxes = my_scene->Add(vglx::InstancedMesh::Create(
 *   geometry, material, 2500
 * );
 *
 * for (auto i = 0; i < 50; ++i) {
 *   for (auto j = 0; j < 50; ++j) {
 *     Transform3 t {};
 *     t.SetPosition({i * 2.0f - 49.0f, j * 2.0f - 49.0f, 0.0f});
 *     boxes->SetTransformAt(j * 50 + i, t);
 *   }
 * }
 * @endcode
 *
 * Out-of-range indices are invalid and result in undefined behavior. Culling
 * is performed using a single bounding volume that determines visibility of
 * the instance set as a whole.
 *
 * @ingroup SceneGroup
 */
class VGLX_EXPORT InstancedMesh : public Mesh {
public:
    /**
     * @brief Constructs an instanced mesh.
     *
     * @param geometry Shared geometry used for every instance.
     * @param material Shared material used for every instance.
     * @param count Number of instances to allocate.
     */
    InstancedMesh(
        std::shared_ptr<Geometry> geometry,
        std::shared_ptr<Material> material,
        std::size_t count
    );

    /**
     * @brief Creates an instance of @ref InstancedMesh.
     *
     * @param geometry Shared geometry used across all instances.
     * @param material Shared material used across all instances.
     * @param count Number of instances to allocate.
     */
    [[nodiscard]] static auto Create(
        std::shared_ptr<Geometry> geometry,
        std::shared_ptr<Material> material,
        std::size_t count
    ) -> std::unique_ptr<InstancedMesh> {
        return std::make_unique<InstancedMesh>(geometry, material, count);
    }

    /**
     * @brief Identifies this node as @ref Node::Type "Node::Type::InstancedMesh".
     */
    [[nodiscard]] auto GetNodeType() const -> Node::Type override {
        return Node::Type::InstancedMesh;
    }

    /// @brief Returns the number of allocated instances.
    [[nodiscard]] auto Count() -> size_t { return count_; }

    /**
     * @brief Returns the per-instance color at the given index.
     *
     * @param idx Instance index.
     */
    [[nodiscard]] auto GetColorAt(std::size_t idx) -> const Color;

    /**
     * @brief Returns the per-instance transform matrix at the given index.
     *
     * @param idx Instance index.
     */
    [[nodiscard]] auto GetTransformAt(std::size_t idx) -> const Matrix4;

    /**
     * @brief Sets the per-instance color at the given index.
     *
     * @param idx Instance index.
     * @param color New color for the instance.
     */
    auto SetColorAt(std::size_t idx, const Color& color) -> void;

    /**
     * @brief Sets the per-instance transform at the given index.
     *
     * @param idx Instance index.
     * @param matrix A world-space transform matrix for the instance.
     */
    auto SetTransformAt(std::size_t idx, const Matrix4& matrix) -> void;

    /**
     * @brief Convenience overload that accepts a @ref Transform3.
     *
     * @param idx Instance index.
     * @param transform World-space transform object to assign.
     */
    auto SetTransformAt(std::size_t idx, Transform3& transform) -> void;

    /**
     * @brief Computes a bounding box that encloses all instances.
     */
    auto BoundingBox() -> Box3 override;

    /**
     * @brief Computes a bounding sphere that encloses all instances.
     */
    auto BoundingSphere() -> Sphere override;

    ~InstancedMesh() override;

private:
    /// @cond INTERNAL
    std::vector<Color> colors_;
    std::vector<Matrix4> transforms_;

    std::size_t count_;

    friend class GLBuffers;
    class Impl;
    std::unique_ptr<Impl> impl_;
    /// @endcond
};

}