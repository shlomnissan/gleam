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
 * @brief Base class for scene nodes that can be drawn.
 *
 * Renderable extends @ref Node with the interface the renderer needs to issue a
 * draw call: a @ref Geometry, a @ref Material, and bounding volumes used for
 * visibility culling. It is the common base for all drawable node types, such
 * as @ref Mesh, @ref InstancedMesh, and @ref Sprite.
 *
 * This class is not instantiated directly. Use one of the concrete renderable
 * types, or derive from it to implement a custom drawable.
 *
 * @ingroup SceneGroup
 */
class VGLX_EXPORT Renderable : public Node {
public:
    /// @brief When `true` this object participates in view frustum culling.
    bool frustum_culled {true};

    /// @brief When `true` this object casts shadows onto shadow receivers.
    bool cast_shadow {false};

    /// @brief When `true` this object receives shadows from shadow casters.
    bool receive_shadow {false};

    /**
     * @brief Returns the geometry used to draw this object.
     */
    [[nodiscard]] virtual auto GetGeometry() const -> std::shared_ptr<Geometry> = 0;

    /**
     * @brief Returns the material used to draw this object.
     */
    [[nodiscard]] virtual auto GetMaterial() const -> std::shared_ptr<Material> = 0;

    /**
     * @brief Returns the object's bounding box.
     *
     * Defaults to the bounding box of the object's @ref GetGeometry "geometry".
     * Subclasses override this when their effective bounds differ, such as
     * @ref InstancedMesh.
     */
    [[nodiscard]] virtual auto BoundingBox() -> Box3;

    /**
     * @brief Returns the object's bounding sphere.
     *
     * Defaults to the bounding sphere of the object's @ref GetGeometry
     * "geometry". Subclasses override this when their effective bounds differ.
     */
    [[nodiscard]] virtual auto BoundingSphere() -> Sphere;

    /**
     * @brief Identifies this node as @ref Node::Type "Node::Type::Renderable".
     */
    [[nodiscard]] auto GetNodeType() const -> Node::Type override {
        return Node::Type::Renderable;
    }

    /**
     * @brief Returns `true`, identifying this node as renderable.
     */
    [[nodiscard]] auto IsRenderable() const -> bool override { return true; }

    /// @cond INTERNAL
    [[nodiscard]] static auto CanRender(Renderable* r) -> bool;

    [[nodiscard]] static auto InFrustum(Renderable* r, const Frustum& frustum) -> bool;

    [[nodiscard]] static auto IsMeshType(Renderable* r) -> bool;
    /// @endcond

    virtual ~Renderable() override = default;

protected:
    Renderable() = default;
};

}
