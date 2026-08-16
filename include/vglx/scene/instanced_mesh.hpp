/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/geometries/buffer_attribute.hpp"
#include "vglx/math/box3.hpp"
#include "vglx/math/color.hpp"
#include "vglx/math/matrix4.hpp"
#include "vglx/math/sphere.hpp"
#include "vglx/scene/mesh.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace vglx {

/**
 * @brief Renderable node that draws many copies of the same mesh efficiently.
 *
 * InstancedMesh stores a single geometry/material pair and renders it
 * multiple times in a single draw call using per-instance transforms and
 * colors. This dramatically improves performance when drawing large numbers
 * of identical objects by reducing CPU overhead and state changes.
 *
 * Per-instance data is stored in instance-rate @ref BufferAttribute
 * "buffer attributes". Transforms default to identity and colors to white.
 * Each instance is addressed by a zero-based index.
 * Updates through @ref SetTransformAt and @ref SetColorAt
 * mark the underlying attribute for re-upload.
 *
 * Custom per-instance data can be added with @ref AddInstanceAttribute and
 * consumed by declaring a matching attribute in the shader code of a
 * @ref ShaderMaterial. The built-in materials only use the instance
 * transform and color attributes.
 *
 * @code
 * const auto geometry = vglx::BoxGeometry::Create({1.0f, 1.0f, 1.0f});
 * const auto material = vglx::PhongMaterial::Create({.color = 0xFFFFFF});
 *
 * auto boxes = my_scene->Add(vglx::InstancedMesh::Create(
 *   geometry, material, 2500
 * ));
 *
 * for (auto i = 0; i < 50; ++i) {
 *   for (auto j = 0; j < 50; ++j) {
 *     vglx::Transform3 t {};
 *     t.SetPosition({i * 2.0f - 49.0f, j * 2.0f - 49.0f, 0.0f});
 *     boxes->SetTransformAt(j * 50 + i, t.Get());
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
     * Instance transforms are initialized to identity and instance colors
     * to white.
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

    /**
     * @brief Adds a custom per-instance attribute to this mesh.
     *
     * The attribute must have an instance rate, a unique name within the
     * mesh and an element count that matches the instance count. Attributes
     * that violate these constraints are reported and rejected.
     *
     * @param attribute Buffer attribute to add.
     */
    auto AddInstanceAttribute(std::shared_ptr<BufferAttribute> attribute) -> void;

    /**
     * @brief Returns the list of instance attributes stored on this mesh.
     */
    [[nodiscard]] auto GetInstanceAttributes() const -> const std::vector<std::shared_ptr<BufferAttribute>>& { return attributes_; }

    /**
     * @brief Returns the instance attribute with the given name.
     *
     * @param name Attribute name to look up.
     * @return The matching attribute, or `nullptr` if none exists.
     */
    [[nodiscard]] auto GetInstanceAttribute(std::string_view name) const -> std::shared_ptr<BufferAttribute>;

    /// @brief Returns the number of allocated instances.
    [[nodiscard]] auto GetCount() const -> size_t { return count_; }

    /**
     * @brief Returns the per-instance transform matrix at the given index.
     *
     * @param idx Instance index.
     */
    [[nodiscard]] auto TransformAt(std::size_t idx) const -> Matrix4;

    /**
     * @brief Returns the per-instance color at the given index.
     *
     * @param idx Instance index.
     */
    [[nodiscard]] auto ColorAt(std::size_t idx) const -> Color;

    /**
     * @brief Sets the per-instance transform at the given index.
     *
     * @param idx Instance index.
     * @param matrix A world-space transform matrix for the instance.
     */
    auto SetTransformAt(std::size_t idx, const Matrix4& matrix) -> void;

    /**
     * @brief Sets the per-instance color at the given index.
     *
     * @param idx Instance index.
     * @param color New color for the instance.
     */
    auto SetColorAt(std::size_t idx, const Color& color) -> void;

    /**
     * @brief Computes a bounding box that encloses all instances.
     */
    auto BoundingBox() -> Box3 override;

    /**
     * @brief Computes a bounding sphere that encloses all instances.
     */
    auto BoundingSphere() -> Sphere override;

private:
    struct BoundsKey {
        uint32_t transform_version;
        uint32_t position_version;
        std::string position_uuid;
        auto operator==(const BoundsKey&) const -> bool = default;
    };

    /// @cond INTERNAL
    std::vector<std::shared_ptr<BufferAttribute>> attributes_ {};

    std::shared_ptr<BufferAttribute> transforms_attr_ {};

    std::shared_ptr<BufferAttribute> colors_attr_ {};

    size_t count_;

    std::optional<std::pair<BoundsKey, Box3>> bounding_box_ {};

    std::optional<std::pair<BoundsKey, Sphere>> bounding_sphere_ {};
    /// @endcond
};

}
