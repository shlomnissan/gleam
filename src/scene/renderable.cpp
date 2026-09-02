/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/scene/renderable.hpp"

#include "utilities/logger.hpp"

namespace vglx {

auto Renderable::BoundingBox() -> Box3 {
    return GetGeometry()->BoundingBox();
}

auto Renderable::BoundingSphere() -> Sphere {
    return GetGeometry()->BoundingSphere();
}

auto Renderable::CanRender(Renderable* r) -> bool {
    const auto geometry = r->GetGeometry();
    const auto material = r->GetMaterial();

    if (geometry == nullptr || material == nullptr) {
        return false;
    }

    if (geometry->Disposed()) {
        Logger::Log(LogLevel::Error, "Skipped rendering a node with disposed geometry {}", *r);
        return false;
    }

    if (geometry->VertexCount() == 0) {
        Logger::Log(LogLevel::Error, "Skipped node with no geometry data {}", *r);
        return false;
    }

    if (!geometry->HasPositions()) {
        Logger::Log(LogLevel::Error, "Skipped node with no vertex positions {}", *r);
        return false;
    }

    const auto node_type = r->GetNodeType();
    const auto material_type = material->GetType();

    if (node_type == Node::Type::Billboard && material_type != Material::Type::BillboardMaterial) {
        Logger::Log(LogLevel::Error, "Skipped billboard with non-billboard material {}", *r);
        return false;
    }

    if (material_type == Material::Type::BillboardMaterial && node_type != Node::Type::Billboard) {
        Logger::Log(LogLevel::Error, "Skipped non-billboard node with billboard material {}", *r);
        return false;
    }

    return true;
}

auto Renderable::InFrustum(Renderable* r, const Frustum& frustum) -> bool {
    auto bounding_sphere = r->BoundingSphere();
    bounding_sphere.ApplyTransform(r->GetWorldTransform());
    return frustum.IntersectsWithSphere(bounding_sphere);
}

auto Renderable::IsMeshType(Renderable* r) -> bool {
    return r->GetNodeType() == Node::Type::Mesh ||
           r->GetNodeType() == Node::Type::InstancedMesh;
}

}
