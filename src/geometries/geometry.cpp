/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/geometries/geometry.hpp"

#include "vglx/math/utilities.hpp"

#include "utilities/logger.hpp"

#include <ranges>
#include <cassert>
#include <numeric>

namespace vglx {

auto Geometry::SetAttribute(const GeometryAttribute &attribute) -> void {
    using enum VertexAttributeType;
    if (attribute.type == Position) assert(attribute.item_size == 3);
    if (attribute.type == Normal) assert(attribute.item_size == 3);
    if (attribute.type == UV) assert(attribute.item_size == 2);
    if (attribute.type == Color) assert(attribute.item_size == 3);

    assert(attribute.type != InstanceColor);
    assert(attribute.type != InstanceTransform);

    auto idx = std::to_underlying(attribute.type);
    if (HasAttribute(attribute.type)) {
        Logger::Log(LogLevel::Warning, "Vertex attribute {} already exists", idx);
        return;
    }

    attributes_[std::to_underlying(attribute.type)] = attribute;
}

auto Geometry::HasAttribute(VertexAttributeType type) const -> bool {
    return attributes_[std::to_underlying(type)].type != VertexAttributeType::None;
}

auto Geometry::VertexCount() const -> size_t {
    if (vertex_data_.empty() || attributes_.empty() || Stride() == 0) {
        return 0;
    }
    return vertex_data_.size() / Stride();
}

auto Geometry::Stride() const -> size_t {
    return std::ranges::fold_left(
        attributes_ | std::views::transform(&GeometryAttribute::item_size),
        0,
        std::plus {}
    );
}

auto Geometry::BoundingBox() -> Box3 {
    if (!bounding_box_.has_value()) CreateBoundingBox();
    return bounding_box_.value();
}

auto Geometry::BoundingSphere() -> Sphere {
    if (!bounding_sphere_.has_value()) CreateBoundingSphere();
    return bounding_sphere_.value();
}

auto Geometry::CreateBoundingBox() -> void {
    using enum VertexAttributeType;
    if (VertexCount() == 0 || !HasAttribute(Position)) {
        Logger::Log(LogLevel::Error, "Failed to create a bounding box");
        return;
    }

    bounding_box_ = Box3 {};
    auto stride = Stride();
    for (auto i = 0; i < vertex_data_.size(); i += stride) {
        bounding_box_->ExpandWithPoint({
            vertex_data_[i],
            vertex_data_[i + 1],
            vertex_data_[i + 2]
        });
    }
}

auto Geometry::CreateBoundingSphere() -> void {
    using enum VertexAttributeType;
    if (VertexCount() == 0 || !HasAttribute(Position)) {
        Logger::Log(LogLevel::Error, "Failed to create a bounding sphere");
        return;
    }

    auto center = BoundingBox().Center();
    auto stride = Stride();
    auto max_distance_squared = 0.0f;
    for (auto i = 0; i < vertex_data_.size(); i += stride) {
        auto point = Vector3 {
            vertex_data_[i],
            vertex_data_[i + 1],
            vertex_data_[i + 2]
        };

        max_distance_squared = std::max(
            max_distance_squared,
            (center - point).LengthSquared()
        );
    }

    bounding_sphere_ = Sphere {center, math::Sqrt(max_distance_squared)};
}

}