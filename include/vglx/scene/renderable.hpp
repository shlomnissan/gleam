/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/geometries/geometry.hpp"
#include "vglx/materials/material.hpp"
#include "vglx/math/frustum.hpp"
#include "vglx/scene/node.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Internal base for anything renderable.
 *
 * Not part of the public API. Provides the minimal interface
 * needed by the renderer (geometry, material).
 *
 * @cond INTERNAL
 */
class VGLX_EXPORT Renderable : public Node {
public:
    virtual ~Renderable() override = default;

    [[nodiscard]] virtual auto GetGeometry() -> std::shared_ptr<Geometry> = 0;

    [[nodiscard]] virtual auto GetMaterial() -> std::shared_ptr<Material> = 0;

    [[nodiscard]] virtual auto BoundingBox() -> Box3;

    [[nodiscard]] virtual auto BoundingSphere() -> Sphere;

    [[nodiscard]] auto GetNodeType() const -> Node::Type override {
        return Node::Type::Renderable;
    }

    [[nodiscard]] auto IsRenderable() const -> bool override { return true; }

    [[nodiscard]] static auto CanRender(Renderable* r) -> bool;

    [[nodiscard]] static auto InFrustum(Renderable* r, const Frustum& frustum) -> bool;

    [[nodiscard]] static auto IsMeshType(Renderable* r) -> bool;

protected:
    Renderable() = default;
};
/// @endcond

}
