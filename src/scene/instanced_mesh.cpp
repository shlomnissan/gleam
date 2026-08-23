/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/scene/instanced_mesh.hpp"

#include "vglx/math/vector4.hpp"

#include "utilities/assert.hpp"
#include "utilities/logger.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <format>

namespace vglx {

namespace {

auto get_transform_at(std::size_t idx, const std::shared_ptr<BufferAttribute>& attribute) -> Matrix4 {
    auto& data = attribute->GetData();
    auto i = idx * 16;
    if (i + 16 > data.size()) {
        Logger::Log(LogLevel::Error, "Failed to read transform. Range exceeds data size");
        return Matrix4::Identity();
    }
    return Matrix4 {
        Vector4 {data[i + 0],  data[i + 1],  data[i + 2],  data[i + 3]},
        Vector4 {data[i + 4],  data[i + 5],  data[i + 6],  data[i + 7]},
        Vector4 {data[i + 8],  data[i + 9],  data[i + 10], data[i + 11]},
        Vector4 {data[i + 12], data[i + 13], data[i + 14], data[i + 15]},
    };
}

}

InstancedMesh::InstancedMesh(
    std::shared_ptr<Geometry> geometry,
    std::shared_ptr<Material> material,
    std::size_t count
) : Mesh(geometry, material), count_(count), draw_count_(count) {
    if (count_ == 0) {
        count_ = 1;
        draw_count_ = 1;
        Logger::Log(
            LogLevel::Warning,
            "Instanced mesh must be initialized with at least one element, "
            "zero element count provided, defaulting to one"
        );
    }

    auto transforms_staging = std::vector<float>(count_ * 16, 0.0f);
    for (auto i = std::size_t{0}; i < count_; ++i) {
        transforms_staging[i * 16 + 0] = 1.0f;
        transforms_staging[i * 16 + 5] = 1.0f;
        transforms_staging[i * 16 + 10] = 1.0f;
        transforms_staging[i * 16 + 15] = 1.0f;
    }

    transforms_attr_ = BufferAttribute::Create({
        .name = BufferAttribute::kInstanceTransform,
        .format = BufferAttribute::Format::Float32x16,
        .rate = BufferAttribute::Rate::Instance
    }, std::move(transforms_staging));

    auto colors_staging = std::vector<float>(count_ * 3, 1.0f);

    colors_attr_ = BufferAttribute::Create({
        .name = BufferAttribute::kInstanceColor,
        .format = BufferAttribute::Format::Float32x3,
        .rate = BufferAttribute::Rate::Instance
    }, std::move(colors_staging));

    attributes_.emplace_back(transforms_attr_);
    attributes_.emplace_back(colors_attr_);
}

auto InstancedMesh::GetInstanceAttribute(std::string_view name) const -> std::shared_ptr<BufferAttribute> {
    if (name == BufferAttribute::kInstanceTransform) { return transforms_attr_; }
    if (name == BufferAttribute::kInstanceColor) { return colors_attr_; }
    auto it = std::ranges::find(attributes_, name, &BufferAttribute::name);
    return it != attributes_.end() ? *it : nullptr;
}

auto InstancedMesh::AddInstanceAttribute(std::shared_ptr<BufferAttribute> attribute) -> void {
    if (attribute == nullptr) return;

    auto error = [name = attribute->name](std::string_view message) {
        Logger::Log(LogLevel::Error, "Failed to add attribute {}. {}", name, message);
    };

    if (attribute->Disposed()) {
        error("The attribute is marked as disposed");
        return;
    }

    if (attribute->rate == BufferAttribute::Rate::Vertex) {
        error("Vertex attributes should be added to Geometry objects");
        return;
    }

    if (!attribute->IsValid()) {
        error("Invalid attribute, missing name or data");
        return;
    }

    if (GetInstanceAttribute(attribute->name) != nullptr) {
        error("An attribute with this name already exists");
        return;
    }

    if (count_ != attribute->ElementCount()) {
        error(std::format("Element count mismatch. Expecting {} elements", count_));
        return;
    }

    attributes_.emplace_back(std::move(attribute));
}

auto InstancedMesh::TransformAt(std::size_t idx) const -> Matrix4 {
    VGLX_ASSERT(idx < count_, "Index exceeds instance count");
    return get_transform_at(idx, transforms_attr_);
}

auto InstancedMesh::ColorAt(std::size_t idx) const -> Color {
    VGLX_ASSERT(idx < count_, "Index exceeds instance count");
    auto& data = colors_attr_->GetData();
    if (idx * 3 + 3 > data.size()) {
        Logger::Log(LogLevel::Error, "Failed to read color. Range exceeds data size");
        return {1.0f, 1.0f, 1.0f};
    }
    return Color { data[idx * 3 + 0], data[idx * 3 + 1], data[idx * 3 + 2] };
}

auto InstancedMesh::SetDrawCount(size_t draw_count) -> void {
    if (draw_count > count_) {
        Logger::Log(
            LogLevel::Warning,
            "Instanced mesh draw count cannot exceed the initial allocation, defaulting to max elements {}",
            count_
        );
    }
    draw_count_ = std::min(draw_count, count_);
}

auto InstancedMesh::SetTransformAt(std::size_t idx, const Matrix4& matrix) -> void {
    VGLX_ASSERT(idx < count_, "Index exceeds instance count");
    transforms_attr_->Write(idx * 16, std::bit_cast<std::array<float, 16>>(matrix));
}

auto InstancedMesh::SetColorAt(std::size_t idx, const Color& color) -> void {
    VGLX_ASSERT(idx < count_, "Index exceeds instance count");
    colors_attr_->Write(idx * 3, std::bit_cast<std::array<float, 3>>(color));
}

auto InstancedMesh::BoundingBox() -> Box3 {
    auto position_attr = GetGeometry()->GetAttribute(BufferAttribute::kPosition);
    if (position_attr == nullptr) {
        return {};
    }

    auto key = BoundsKey {
        .transform_version = transforms_attr_->GetVersion(),
        .position_version = position_attr->GetVersion(),
        .draw_count = draw_count_,
        .position_uuid = position_attr->UUID()
    };

    if (bounding_box_ && bounding_box_->first == key) {
        return bounding_box_->second;
    }

    auto base = GetGeometry()->BoundingBox();
    if (base.IsEmpty()) {
        return {};
    }

    auto box = Box3 {};
    for (auto i = std::size_t {0}; i < draw_count_; ++i) {
        auto b = base;
        b.ApplyTransform(get_transform_at(i, transforms_attr_));
        box.Union(b);
    }

    bounding_box_ = {key, box};
    return bounding_box_->second;
}

auto InstancedMesh::BoundingSphere() -> Sphere {
    auto position_attr = GetGeometry()->GetAttribute(BufferAttribute::kPosition);
    if (position_attr == nullptr) {
        return {};
    }

    auto key = BoundsKey {
        .transform_version = transforms_attr_->GetVersion(),
        .position_version = position_attr->GetVersion(),
        .draw_count = draw_count_,
        .position_uuid = position_attr->UUID()
    };

    if (bounding_sphere_ && bounding_sphere_->first == key) {
        return bounding_sphere_->second;
    }

    auto base = GetGeometry()->BoundingSphere();
    if (base.IsEmpty()) {
        return {};
    }

    auto sphere = Sphere {};
    for (auto i = std::size_t {0}; i < draw_count_; ++i) {
        auto s = base;
        s.ApplyTransform(get_transform_at(i, transforms_attr_));
        sphere.Union(s);
    }

    bounding_sphere_ = {key, sphere};
    return bounding_sphere_->second;
}

}
