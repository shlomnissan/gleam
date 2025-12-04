/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/nodes/mesh.hpp"

#include "vglx/geometries/wireframe_geometry.hpp"

namespace vglx {

auto Mesh::SetGeometry(std::shared_ptr<Geometry> geometry) -> void {
    geometry_ = geometry;
    wireframe_geometry_ = nullptr;
}

auto Mesh::GetWireframeGeometry() -> std::shared_ptr<Geometry> {
    if (geometry_->primitive != Geometry::PrimitiveType::Triangles) {
        return geometry_;
    }

    if (wireframe_geometry_ == nullptr) {
        wireframe_geometry_ = WireframeGeometry::Create(geometry_.get());
    }

    return wireframe_geometry_;
}

}