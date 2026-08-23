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
    const auto level = LogLevel::Error;
    const auto geometry = r->GetGeometry();
    const auto material = r->GetMaterial();
    const auto mat_type = material->GetType();
    const auto node_type = r->GetNodeType();

    if (geometry == nullptr) {
        Logger::Log(level, "Skipped rendering a node with invalid geometry {}", *r);
        return false;
    }

    if (geometry->Disposed()) {
        Logger::Log(level, "Skipped rendering a node with disposed geometry {}", *r);
        return false;
    }

    if (geometry->VertexCount() == 0) {
        Logger::Log(level, "Skipped node with no geometry data {}", *r);
        return false;
    }

    if (!geometry->HasPositions()) {
        Logger::Log(level, "Skipped node with no vertex positions {}", *r);
        return false;
    }

    if (material == nullptr) {
        Logger::Log(level, "Skipped node with invalid material {}", *r);
        return false;
    }

    if (node_type == Node::Type::Billboard && mat_type != Material::Type::BillboardMaterial) {
        Logger::Log(level, "Skipped billboard with non-billboard material {}", *r);
        return false;
    }

    if (mat_type == Material::Type::BillboardMaterial && node_type != Node::Type::Billboard) {
        Logger::Log(level, "Skipped non-billboard node with billboard material {}", *r);
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
