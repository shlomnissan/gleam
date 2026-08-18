/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/scene/mesh.hpp"

#include "vglx/geometries/wireframe_geometry.hpp"

#include <utility>

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
        auto wireframe = WireframeGeometry::Create(geometry_.get());
        if (wireframe->GetIndexData().empty()) {
            return geometry_;
        }
        wireframe_geometry_ = std::move(wireframe);
    }

    return wireframe_geometry_;
}

}