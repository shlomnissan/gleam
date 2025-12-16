/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/scene/instanced_mesh.hpp"

#include "scene/instanced_mesh_impl.hpp"

#include <cassert>

namespace vglx {

InstancedMesh::InstancedMesh(
    std::shared_ptr<Geometry> geometry,
    std::shared_ptr<Material> material,
    std::size_t count
): Mesh(geometry, material), count_(count), impl_(std::make_unique<Impl>()) {
    transforms_.resize(count);
    colors_.resize(count);
}

auto InstancedMesh::GetColorAt(std::size_t idx) -> const Color {
    assert(idx <= count_);
    return colors_[idx];
}

auto InstancedMesh::GetTransformAt(std::size_t idx) -> const Matrix4 {
    assert(idx <= count_);
    return transforms_[idx];
}

auto InstancedMesh::SetColorAt(std::size_t idx, const Color& color) -> void {
    assert(idx <= count_);
    colors_[idx] = color;
    impl_->colors_touched = true;
}

auto InstancedMesh::SetTransformAt(std::size_t idx, const Matrix4& matrix) -> void {
    assert(idx <= count_);
    transforms_[idx] = matrix;
    impl_->transforms_touched = true;
    impl_->bounding_box_touched = true;
    impl_->bounding_sphere_touched = true;
}

auto InstancedMesh::SetTransformAt(std::size_t idx, Transform3& transform) -> void {
    SetTransformAt(idx, transform.Get());
}

auto InstancedMesh::BoundingBox() -> Box3 {
    if (impl_->bounding_box_touched) {
        const auto base = GetGeometry()->BoundingBox();
        if (!base.IsEmpty() && count_ > 0) {
            impl_->bounding_box.Reset();
            for (auto i = 0; i < count_; ++i) {
                auto box = base;
                box.ApplyTransform(GetTransformAt(i));
                impl_->bounding_box.Union(box);
            }
        }
        impl_->bounding_box_touched = false;
    }
    return impl_->bounding_box;
}

auto InstancedMesh::BoundingSphere() -> Sphere {
    if (impl_->bounding_sphere_touched) {
        const auto base = GetGeometry()->BoundingSphere();
        if (!base.IsEmpty() && count_ > 0) {
            impl_->bounding_sphere.Reset();
            for (auto i = 0; i < count_; ++i) {
                auto sphere = base;
                sphere.ApplyTransform(GetTransformAt(i));
                impl_->bounding_sphere.Union(sphere);
            }
        }
        impl_->bounding_sphere_touched = false;
    }
    return impl_->bounding_sphere;
}

InstancedMesh::~InstancedMesh() = default;

}