/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/core/disposable.hpp"
#include "vglx/core/identity.hpp"
#include "vglx/math/box3.hpp"
#include "vglx/math/sphere.hpp"
#include "vglx/math/utilities.hpp"

#include <array>
#include <memory>
#include <optional>
#include <utility>

namespace vglx {

/**
 * @brief Represents GPU-ready mesh data with vertex and index buffers.
 *
 * Geometry is the core unit of renderable mesh data in VGLX. It stores raw
 * vertex and index buffers along with attribute metadata that describes how
 * the data should be interpreted by the renderer (positions, normals, UVs,
 * tangents, per-instance data, and so on).
 *
 * A geometry can be rendered using different primitive types (triangles,
 * lines, line loops) and provides access to derived bounding volumes such as
 * axis-aligned bounding boxes and bounding spheres, useful for frustum
 * culling or collision queries.
 *
 * Instances are usually created via the static @ref Geometry::Create factory
 * methods and configured with vertex attributes using @ref Geometry::SetAttribute.
 *
 * @code
 * auto geometry = vglx::Geometry::Create({
 *   // 3 vertices, XYZ position only:
 *   0.5f, -0.5f, 0.0f,
 *   0.0f,  0.5f, 0.0f,
 *  -0.5f, -0.5f, 0.0f,
 * });
 *
 * geometry->SetAttribute({vglx::Geometry::VertexAttributeType::Position, 3});
 *
 * auto material = vglx::PhongMaterial::Create({.color = 0x049EF4});
 *
 * my_scene->Add(vglx::Mesh::Create(geometry, material));
 * @endcode
 *
 * @ingroup GeometryGroup
 */
class VGLX_EXPORT Geometry : public Disposable, public Identity {
public:
    /**
     * @brief Enumerates supported vertex attribute semantics.
     *
     * Each value identifies the purpose of a vertex attribute stream, such as
     * positions, normals, UVs, or per-instance transforms. Attributes are
     * described via @ref Geometry::VertexAttribute and stored in the
     * @ref Geometry::Attributes "attributes array".
     */
    enum class VertexAttributeType {
        Position = 0, ///< Vertex position in object space.
        Normal = 1, ///< Vertex normal in object space.
        UV = 2, ///< Texture coordinates.
        Tangent = 3, ///< Tangent vector for normal mapping.
        Color = 4, ///< Per-vertex color.
        InstanceColor = 5, ///< Per-instance color.
        InstanceTransform = 6, ///< Per-instance transform matrix.
        None ///< Sentinel value indicating no attribute.
    };

    /**
     * @brief Primitive topology used when rendering this geometry.
     *
     * Controls how the vertex and index data are interpreted by the renderer.
     */
    enum class PrimitiveType {
        Triangles, ///< Render as triangle list.
        Lines, ///< Render as line list.
        LineLoop ///< Render as a closed line loop.
    };

    /**
     * @brief Describes a single vertex attribute stream.
     *
     * Each attribute entry specifies the semantic @ref type and the number of
     * components per vertex `item_size` (for example, 3 for a vec3 position).
     * Attributes are stored in a fixed-size array indexed by
     * @ref Geometry::VertexAttributeType.
     */
    struct VertexAttribute {
        VertexAttributeType type {VertexAttributeType::None}; ///< Attribute semantic.
        unsigned int item_size {0}; ///< Number of float components per vertex.
    };

    /// @brief Total number of supported attribute slots.
    static constexpr int AttributesLength = std::to_underlying(VertexAttributeType::None);

    /// @brief Convenience alias for the attribute array type.
    using AttributesType = std::array<vglx::Geometry::VertexAttribute, AttributesLength>;

    /// @brief Primitive topology used by this geometry (triangles by default).
    PrimitiveType primitive { PrimitiveType::Triangles };

    /// @brief Renderer-specific identifier for the underlying GPU resource.
    unsigned int renderer_id = 0;

    /**
     * @brief Constructs an empty geometry.
     *
     * The geometry is created without any vertex or index data. Data and
     * attributes can be assigned later using the appropriate APIs.
     */
    Geometry() = default;

    /**
     * @brief Constructs a geometry from vertex and index data.
     *
     * The vertex buffer is populated from `vertex_data`, and the optional
     * index buffer from `index_data`. Attribute metadata must be configured
     * separately using @ref SetAttribute.
     *
     * @param vertex_data Interleaved vertex data stored as floats.
     * @param index_data Optional index buffer (empty for non-indexed geometry).
     */
    Geometry(
        const std::vector<float>& vertex_data,
        const std::vector<unsigned int>& index_data
    ) : vertex_data_(vertex_data), index_data_(index_data) {}

    /**
     * @brief Creates a shared instance of an empty @ref Geometry.
     *
     * The resulting geometry has no vertex or index data and must be populated
     * and configured before rendering.
     */
    [[nodiscard]] static auto Create() -> std::shared_ptr<Geometry> {
        return std::make_shared<Geometry>();
    }

    /**
     * @brief Creates a shared instance of @ref Geometry from vertex and index data.
     *
     * @param vertex_data Interleaved vertex data stored as floats.
     * @param index_data Optional index data; leave empty for non-indexed geometry.
     */
    [[nodiscard]] static auto Create(
        const std::vector<float>& vertex_data,
        const std::vector<unsigned int>& index_data = {}
    ) -> std::shared_ptr<Geometry> {
        return std::make_shared<Geometry>(vertex_data, index_data);
    }

    /**
     * @brief Registers a vertex attribute on this geometry.
     *
     * The attribute is stored in the internal attribute array and used by the
     * renderer to interpret the vertex buffer layout. Multiple attributes can
     * be configured as long as their combined stride matches the vertex data.
     *
     * @param attribute Vertex attribute descriptor to add or update.
     */
    auto SetAttribute(const Geometry::VertexAttribute& attribute) -> void;

    /**
     * @brief Returns a read-only reference to the vertex buffer.
     *
     * The data is stored as a flat array of floats, typically interleaved
     * according to the configured attributes.
     */
    [[nodiscard]] auto VertexData() const -> const std::vector<float>& {
        return vertex_data_;
    }

    /**
     * @brief Returns a read-only reference to the index buffer.
     *
     * If the geometry is non-indexed, this buffer may be empty.
     */
    [[nodiscard]] auto IndexData() const -> const std::vector<unsigned int>& {
        return index_data_;
    }

    /**
     * @brief Returns the number of indices stored in the index buffer.
     *
     * For non-indexed geometry, this value is zero.
     */
    [[nodiscard]] auto IndexCount() const -> size_t {
        return index_data_.size();
    }

    /**
     * @brief Returns the array of vertex attribute descriptors.
     *
     * The array is indexed by @ref Geometry::VertexAttributeType values in the
     * range `[0, AttributesLength)`.
     */
    [[nodiscard]] auto Attributes() const -> const AttributesType& {
        return attributes_;
    }

    /**
     * @brief Returns the number of vertices in the geometry.
     *
     * The vertex count is derived from the size of the vertex buffer and the
     * configured stride.
     */
    [[nodiscard]] auto VertexCount() const -> size_t;

    /**
     * @brief Returns the vertex stride in number of floats.
     *
     * The stride is computed from the active vertex attributes and describes
     * how many floats form a single vertex in the interleaved buffer.
     */
    [[nodiscard]] auto Stride() const -> size_t;

    /**
     * @brief Checks whether an attribute of the given type is present.
     *
     * @param type Attribute semantic to query.
     */
    [[nodiscard]] auto HasAttribute(VertexAttributeType type) const -> bool;

    /**
     * @brief Returns an axis-aligned bounding box enclosing the geometry.
     *
     * The bounding box is computed lazily from the vertex positions and cached
     * internally. Subsequent calls reuse the cached result until the geometry
     * data changes.
     */
    [[nodiscard]] auto BoundingBox() -> Box3;

    /**
     * @brief Returns a bounding sphere enclosing the geometry.
     *
     * The bounding sphere is computed lazily from the vertex positions and
     * cached internally. Subsequent calls reuse the cached result until the
     * geometry data changes.
     */
    [[nodiscard]] auto BoundingSphere() -> Sphere;

    virtual ~Geometry();

protected:
    /// @cond INTERNAL
    std::vector<float> vertex_data_;
    std::vector<unsigned int> index_data_;

    std::optional<Box3> bounding_box_;
    std::optional<Sphere> bounding_sphere_;

    AttributesType attributes_ {};

    auto CreateBoundingBox() -> void;

    auto CreateBoundingSphere() -> void;
    /// @endcond
};

}