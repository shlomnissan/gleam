/*
===========================================================================
  GLEAM ENGINE https://gleamengine.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "gleam_export.h"
#include "gleam/core/disposable.hpp"
#include "gleam/core/identity.hpp"
#include "gleam/math/box3.hpp"
#include "gleam/math/sphere.hpp"
#include "gleam/math/utilities.hpp"

#include <memory>
#include <optional>
#include <vector>

namespace gleam {

/**
 * @brief Enum representing the type of a geometry attribute.
 */
enum class GeometryAttributeType {
    Position,  ///< Position attribute.
    Normal,    ///< Normal attribute.
    UV,        ///< UV attribute.
};

/**
 * @brief Enum representing the primitive type used for geometry rendering.
 */
enum class GeometryPrimitiveType {
    Triangles,  ///< Render triangles.
    Lines,      ///< Render lines.
    LineLoop    ///< Render a line loop.
};

/**
 * @brief Structure representing a geometry attribute.
 */
struct GeometryAttribute {
    /// @brief The type of the attribute.
    GeometryAttributeType type;
    /// @brief The number of components per vertex.
    unsigned int item_size;
};

/**
 * @brief Class representing a 3D geometry with vertex and index data.
 */
class GLEAM_EXPORT Geometry : public Disposable, public Identity {
public:
    /// @brief The primitive type of the geometry (e.g., triangles, lines).
    GeometryPrimitiveType primitive { GeometryPrimitiveType::Triangles };

    /// @brief Renderer-specific identifier assigned by the graphics API
    unsigned int renderer_id = 0;

    /**
     * @brief Default construction.
     */
    Geometry() = default;

    /**
     * @brief Constructs a Geometry object with given vertex and index data.
     *
     * @param vertex_data A vector containing the vertex data.
     * @param index_data A vector containing the index data (optional).
     */
    Geometry(
        const std::vector<float>& vertex_data,
        const std::vector<unsigned int>& index_data
    ) : vertex_data_(vertex_data), index_data_(index_data) {}

    /**
     * @brief Creates a shared pointer to a Geometry object with specified vertex and index data.
     *
     * @param vertex_data A vector containing the vertex data.
     * @param index_data A vector containing the index data (optional).
     * @return std::shared_ptr<Geometry> A shared pointer to the newly created Geometry object.
     */
    [[nodiscard]] static auto Create(
        const std::vector<float>& vertex_data,
        const std::vector<unsigned int>& index_data = {}
    ){
        return std::make_shared<Geometry>(vertex_data, index_data);
    }

    /**
     * @brief Gets the vertex data of the geometry.
     *
     * @return const std::vector<float>& A reference to the vector containing the vertex data.
     */
    [[nodiscard]] const auto& VertexData() const { return vertex_data_; }

    /**
     * @brief Calculates the number of vertex elements.
     *
     * @return The vertex count calculated by dividing the size of the vertex data by the stride.
     */
    [[nodiscard]] auto VertexCount() const -> size_t;

    /**
     * @brief Gets the number of indices in the geometry.
     *
     * @return The size of the index data vector.
     */
    [[nodiscard]] auto IndexCount() const -> size_t { return index_data_.size(); }

    /**
     * @brief Gets the index data of the geometry.
     *
     * @return const std::vector<unsigned int>& A reference to the vector containing the index data.
     */
    [[nodiscard]] const auto& IndexData() const { return index_data_; }

    /**
     * @brief Gets the attributes of the geometry.
     *
     * @return const std::vector<GeometryAttribute>& A reference to the vector containing the geometry attributes.
     */
    [[nodiscard]] const auto& Attributes() const { return attributes_; }

    /**
     * @brief Calculates the stride based on existing attributes.
     *
     * @return The stride calculated by the sum of attribute item sizes.
     */
    [[nodiscard]] auto Stride() const -> size_t;

    /**
     * @brief Sets a geometry attribute.
     *
     * @param attribute The attribute to set.
     */
    auto SetAttribute(const GeometryAttribute& attribute) -> void;

    /**
     * @brief Checks if the geometry has a specific attribute.
     *
     * @param type The attribute type to check for.
     * @return bool True if the geometry has the attribute, false otherwise.
     */
    [[nodiscard]] auto HasAttribute(GeometryAttributeType type) const -> bool;

    /**
     * @brief Creates a shared pointer to a default-constructed Geometry object.
     *
     * @return std::shared_ptr<Geometry> A shared pointer to the newly created Geometry object.
     */
    [[nodiscard]] static auto Create() {
        return std::make_shared<Geometry>();
    }

    /**
     * @brief Gets the bounding box of the geometry.
     *
     * @return Box3 The bounding box of the geometry.
     */
    [[nodiscard]] auto BoundingBox() -> Box3;

    /**
     * @brief Gets the bounding sphere of the geometry.
     *
     * @return Sphere The bounding sphere of the geometry.
     */
    [[nodiscard]] auto BoundingSphere() -> Sphere;

    /**
     * @brief Destructor calls the Dispose() method to clean up resources.
     */
    ~Geometry() {
        Dispose();
    }

protected:
    /// @brief The vertex data of the geometry.
    std::vector<float> vertex_data_;

    /// @brief The index data of the geometry.
    std::vector<unsigned int> index_data_;

    /// @brief The bounding box of the geometry.
    std::optional<Box3> bounding_box_;

    /// @brief The bounding sphere of the geometry.
    std::optional<Sphere> bounding_sphere_;

    /// @brief The attributes of the geometry.
    std::vector<GeometryAttribute> attributes_;

    /**
     * @brief Create and cache a Bounding Box object.
     */
    auto CreateBoundingBox() -> void;

    /**
     * @brief Create and cache a Bounding Sphere object.
     */
    auto CreateBoundingSphere() -> void;
};

}