/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/scene/instanced_mesh2.hpp"

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

InstancedMesh2::InstancedMesh2(
    std::shared_ptr<Geometry> geometry,
    std::shared_ptr<Material> material,
    std::size_t count
) : Mesh(geometry, material), count_(count) {
    if (count == 0) {
        Logger::Log(LogLevel::Error, "Instanced mesh must be initialized with at least one element");
        return;
    }

    auto flat = std::vector<float>(count * 16, 0.0f);
    for (auto i = std::size_t{0}; i < count; ++i) {
        flat[i * 16 + 0] = flat[i * 16 + 5] = flat[i * 16 + 10] = flat[i * 16 + 15] = 1.0f;
    }
    attributes_.emplace_back(BufferAttribute::Create({
        .name = BufferAttribute::kInstanceTransform,
        .format = BufferAttribute::Format::Float32x16,
        .rate = BufferAttribute::Rate::Instance
    }, std::move(flat)));

    auto colors = std::vector<float>(count * 3, 1.0f);
    attributes_.emplace_back(BufferAttribute::Create({
        .name = BufferAttribute::kInstanceColor,
        .format = BufferAttribute::Format::Float32x3,
        .rate = BufferAttribute::Rate::Instance
    }, std::move(colors)));
}

auto InstancedMesh2::GetInstanceAttribute(std::string_view name) const -> std::shared_ptr<BufferAttribute> {
    auto it = std::ranges::find(attributes_, name, &BufferAttribute::name);
    return it != attributes_.end() ? *it : nullptr;
}

auto InstancedMesh2::AddInstanceAttribute(std::shared_ptr<BufferAttribute> attribute) -> void {
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
        error("An attribute with this name already exist");
        return;
    }

    if (count_ != attribute->ElementCount()) {
        error(std::format("Element count mismatch. Expecting {} elements", count_));
        return;
    }

    attributes_.emplace_back(std::move(attribute));
    layout_version_++;
}

auto InstancedMesh2::TransformAt(std::size_t idx) const -> Matrix4 {
    VGLX_ASSERT(idx < count_, "Index exceeds instance count");
    auto attribute = GetInstanceAttribute(BufferAttribute::kInstanceTransform);
    if (attribute == nullptr) {
        Logger::Log(LogLevel::Error, "Failed to read transform. Missing transform buffer attribute");
        return Matrix4::Identity();
    }
    return get_transform_at(idx, attribute);
}

auto InstancedMesh2::ColorAt(std::size_t idx) const -> Color {
    VGLX_ASSERT(idx < count_, "Index exceeds instance count");
    auto attribute = GetInstanceAttribute(BufferAttribute::kInstanceColor);
    if (attribute == nullptr) {
        Logger::Log(LogLevel::Error, "Failed to read color. Missing color buffer attribute");
        return {};
    }
    auto& data = attribute->GetData();
    if (idx * 3 + 3 > data.size()) {
        Logger::Log(LogLevel::Error, "Failed to read color. Range exceeds data size");
        return {};
    }

    return Color { data[idx * 3 + 0], data[idx * 3 + 1], data[idx * 3 + 2] };
}

auto InstancedMesh2::SetTransformAt(std::size_t idx, const Matrix4& matrix) -> void {
    VGLX_ASSERT(idx < count_, "Index exceeds instance count");
    auto attribute = GetInstanceAttribute(BufferAttribute::kInstanceTransform);
    if (attribute == nullptr) {
        Logger::Log(LogLevel::Error, "Failed to write transform. Missing transform buffer attribute");
        return;
    }
    attribute->Write(idx * 16, std::bit_cast<std::array<float, 16>>(matrix));
}

auto InstancedMesh2::SetColorAt(std::size_t idx, const Color& color) -> void {
    VGLX_ASSERT(idx < count_, "Index exceeds instance count");
    auto attribute = GetInstanceAttribute(BufferAttribute::kInstanceColor);
    if (attribute == nullptr) {
        Logger::Log(LogLevel::Error, "Failed to write color. Missing color buffer attribute");
        return;
    }
    attribute->Write(idx * 3, std::bit_cast<std::array<float, 3>>(color));
}

auto InstancedMesh2::BoundingBox() -> Box3 {
    auto attribute = GetInstanceAttribute(BufferAttribute::kInstanceTransform);
    if (attribute == nullptr) {
        Logger::Log(LogLevel::Error, "Failed to generate bounding box. Missing transform buffer attribute");
        return {};
    }

    // TODO: include geometry position attribute version in key when it's available

    if (bounding_box_ && bounding_box_->second == attribute->GetVersion()) {
        return bounding_box_->first;
    }

    auto base = GetGeometry()->BoundingBox();
    if (base.IsEmpty()) {
        return {};
    }

    auto box = Box3 {};
    for (auto i = std::size_t {0}; i < count_; ++i) {
        auto b = base;
        b.ApplyTransform(get_transform_at(i, attribute));
        box.Union(b);
    }

    bounding_box_ = {box, attribute->GetVersion()};
    return bounding_box_->first;
}

auto InstancedMesh2::BoundingSphere() -> Sphere {
    auto attribute = GetInstanceAttribute(BufferAttribute::kInstanceTransform);
    if (attribute == nullptr) {
        Logger::Log(LogLevel::Error, "Failed to generate bounding sphere. Missing transform buffer attribute");
        return {};
    }

    // TODO: include geometry position attribute version in key when it's available

    if (bounding_sphere_ && bounding_sphere_->second == attribute->GetVersion()) {
        return bounding_sphere_->first;
    }

    auto base = GetGeometry()->BoundingSphere();
    if (base.IsEmpty()) {
        return {};
    }

    auto sphere = Sphere {};
    for (auto i = std::size_t {0}; i < count_; ++i) {
        auto s = base;
        s.ApplyTransform(get_transform_at(i, attribute));
        sphere.Union(s);
    }

    bounding_sphere_ = {sphere, attribute->GetVersion()};
    return bounding_sphere_->first;
}

}
