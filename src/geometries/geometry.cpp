/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/geometries/geometry.hpp"

#include "vglx/math/utilities.hpp"
#include "vglx/math/vector3.hpp"

#include "utilities/logger.hpp"

#include <algorithm>
#include <cstddef>
#include <format>

namespace vglx {

namespace {

auto create_bounding_box(const std::vector<float>& positions) {
    auto box = Box3 {};
    for (auto i = std::size_t {0}; i + 2 < positions.size(); i += 3) {
        box.ExpandWithPoint({
            positions[i],
            positions[i + 1],
            positions[i + 2]
        });
    }
    return box;
}

auto create_bounding_sphere(const std::vector<float>& positions, const Vector3& center) {
    auto max_distance_squared = 0.0f;
    for (auto i = std::size_t {0}; i + 2 < positions.size(); i += 3) {
        auto point = Vector3 {
            positions[i],
            positions[i + 1],
            positions[i + 2]
        };
        max_distance_squared = std::max(max_distance_squared, (center - point).LengthSquared());
    }
    return Sphere {center, math::Sqrt(max_distance_squared)};
}

}

auto Geometry::AddAttribute(std::shared_ptr<BufferAttribute> attribute) -> void {
    if (attribute == nullptr) return;

    auto error = [name = attribute->name](std::string_view message) {
        Logger::Log(LogLevel::Error, "Failed to add attribute {}. {}", name, message);
    };

    if (attribute->Disposed()) {
        error("Attribute is marked as disposed");
        return;
    }

    if (attribute->rate == BufferAttribute::Rate::Instance) {
        error("Instanced attributes should be added to InstancedMesh objects");
        return;
    }

    if (!attribute->IsValid()) {
        error("Invalid attribute, missing name or data");
        return;
    }

    if (GetAttribute(attribute->name) != nullptr) {
        error("An attribute with this name already exists");
        return;
    }

    auto element_count = VertexCount();
    if (element_count > 0 && element_count != attribute->ElementCount()) {
        error(std::format("Element count mismatch. Expecting {} elements", element_count));
        return;
    }

    attributes_.emplace_back(std::move(attribute));
}

auto Geometry::SetIndices(std::vector<uint32_t> index_data) -> void {
    index_data_ = std::move(index_data);

    auto it = std::ranges::max_element(index_data_);
    max_index_ = it != index_data_.end() ? *it : 0;

    index_version_++;
}

auto Geometry::GetAttribute(std::string_view name) const -> std::shared_ptr<BufferAttribute> {
    auto it = std::ranges::find(attributes_, name, &BufferAttribute::name);
    return it != attributes_.end() ? *it : nullptr;
}

auto Geometry::VertexCount() const -> uint32_t {
    return attributes_.empty() ? 0 : attributes_.front()->ElementCount();
}

auto Geometry::HasPositions() const -> bool {
    return GetAttribute(BufferAttribute::kPosition) != nullptr;
}

auto Geometry::BoundingBox() -> Box3 {
    auto position_attribute = GetAttribute(BufferAttribute::kPosition);
    if (position_attribute == nullptr || position_attribute->GetData().empty()) {
        Logger::Log(LogLevel::Error, "Failed to generate bounding box. Missing vertex position buffer");
        return {};
    }

    if (bounding_box_ && bounding_box_->second == position_attribute->GetVersion()) {
        return bounding_box_->first;
    }

    bounding_box_ = {
        create_bounding_box(position_attribute->GetData()),
        position_attribute->GetVersion()
    };

    return bounding_box_->first;
}

auto Geometry::BoundingSphere() -> Sphere {
    auto position_attribute = GetAttribute(BufferAttribute::kPosition);
    if (position_attribute == nullptr || position_attribute->GetData().empty()) {
        Logger::Log(LogLevel::Error, "Failed to generate bounding sphere. Missing vertex position buffer");
        return {};
    }

    if (bounding_sphere_ && bounding_sphere_->second == position_attribute->GetVersion()) {
        return bounding_sphere_->first;
    }

    bounding_sphere_ = {
        create_bounding_sphere(position_attribute->GetData(), BoundingBox().Center()),
        position_attribute->GetVersion()
    };

    return bounding_sphere_->first;
}

Geometry::~Geometry() {
    Dispose();
}

}
