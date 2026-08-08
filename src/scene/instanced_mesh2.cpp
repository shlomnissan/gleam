/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/scene/instanced_mesh2.hpp"

namespace vglx {

InstancedMesh2::InstancedMesh2(
    std::shared_ptr<Geometry> geometry,
    std::shared_ptr<Material> material,
    std::size_t count
) : Mesh(geometry, material), count_(count) {
    // TODO: implement
}

auto InstancedMesh2::AddInstanceAttribute(std::shared_ptr<BufferAttribute> attribute) -> void {
    // TODO: implement
}

auto InstancedMesh2::TransformAt(std::size_t idx) const -> Matrix4 {
    // TODO: implement
    return {};
}

auto InstancedMesh2::ColorAt(std::size_t idx) const -> Color {
    // TODO: implement
    return {};
}

auto InstancedMesh2::SetTransformAt(std::size_t idx, const Matrix4& matrix) -> void {
    // TODO: implement
}

auto InstancedMesh2::SetColorAt(std::size_t idx, const Color& color) -> void {
    // TODO: implement
}

auto InstancedMesh2::BoundingBox() -> Box3 {
    // TODO: implement
    return {};
}

auto InstancedMesh2::BoundingSphere() -> Sphere {
    // TODO: implement
    return {};
}

}
