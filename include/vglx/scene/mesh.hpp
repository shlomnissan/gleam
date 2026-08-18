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
#include "vglx/scene/renderable.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Renderable node that couples geometry with a material.
 *
 * Mesh represents a draw call in the scene: it owns a shared pointer to a
 * @ref Geometry and a @ref Material and exposes them through the renderable
 * interface. The renderer queries meshes for their geometry, material, and
 * world transform, then issues the appropriate GPU commands. Meshes can also
 * lazily generate a wireframe representation for debugging.
 *
 * @code
 * auto geometry = vglx::BoxGeometry::Create();
 * auto material = vglx::UnlitMaterial::Create({.color = 0x00FFAAu});
 *
 * my_scene->Add(vglx::Mesh::Create(geometry, material));
 * @endcode
 *
 * @ingroup SceneGroup
 */
class VGLX_EXPORT Mesh : public Renderable {
public:
    /**
     * @brief Constructs a mesh.
     *
     * @param geometry Shared pointer to the mesh geometry.
     * @param material Shared pointer to the material used for rendering.
     */
    Mesh(std::shared_ptr<Geometry> geometry, std::shared_ptr<Material> material)
      : geometry_(geometry), material_(material) {}

    /**
     * @brief Creates an instance of @ref Mesh.
     *
     * @param geometry Shared pointer to the mesh geometry.
     * @param material Shared pointer to the material used for rendering.
     */
    [[nodiscard]] static auto Create(
        std::shared_ptr<Geometry> geometry,
        std::shared_ptr<Material> material
    ) -> std::unique_ptr<Mesh> {
        return std::make_unique<Mesh>(geometry, material);
    }

    /**
     * @brief Identifies this node as @ref Node::Type "Node::Type::Mesh".
     */
    [[nodiscard]] auto GetNodeType() const -> Node::Type override {
        return Node::Type::Mesh;
    }

    /**
     * @brief Returns the geometry used by this mesh.
     */
    [[nodiscard]] auto GetGeometry() const -> std::shared_ptr<Geometry> override {
        return geometry_;
    }

    /**
     * @brief Returns the material used by this mesh.
     */
    [[nodiscard]] auto GetMaterial() const -> std::shared_ptr<Material> override {
        return material_;
    }

    /**
     * @brief Replaces the geometry used by this mesh.
     *
     * @param geometry New geometry to assign.
     */
    auto SetGeometry(std::shared_ptr<Geometry> geometry) -> void;

    /**
     * @brief Replaces the material used by this mesh.
     *
     * @param material New material to assign.
     */
    auto SetMaterial(std::shared_ptr<Material> material) -> void { material_ = material; }

    /**
     * @brief Returns a wireframe representation of the current geometry.
     *
     * The wireframe geometry is generated on first use from the mesh's
     * triangle-based geometry and cached for subsequent calls.
     */
    [[nodiscard]] auto GetWireframeGeometry() -> std::shared_ptr<Geometry>;

    ~Mesh() override = default;

private:
    /// @cond INTERNAL
    std::shared_ptr<Geometry> geometry_;
    std::shared_ptr<Geometry> wireframe_geometry_;
    std::shared_ptr<Material> material_;
    /// @endcond
};

}
