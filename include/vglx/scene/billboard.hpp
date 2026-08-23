/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/geometries/geometry.hpp"
#include "vglx/materials/billboard_material.hpp"
#include "vglx/materials/material.hpp"
#include "vglx/math/vector2.hpp"
#include "vglx/scene/renderable.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Camera-facing quad that always faces the active camera.
 *
 * Billboard is a flat unit-sized quad oriented toward the camera at all
 * times. It is usually textured and exposes the minimal Renderable interface
 * so it can be submitted to the renderer. Common use cases include particles,
 * labels, icons, and world-space markers.
 *
 * @code
 * MyNode::MyNode() {
 *   auto texture = LoadTexture("assets/billboard.png");
 *   if (texture.has_value()) {
 *     auto material = vglx::BillboardMaterial::Create({.texture_map = texture.value()});
 *     Add(vglx::Billboard::Create(material))->transform.SetScale(0.5f);
 *   } else {
 *     std::println(stderr, "{}", texture.error());
 *   }
 * }
 * @endcode
 *
 * @ingroup SceneGroup
 */
class VGLX_EXPORT Billboard : public Renderable {
public:
    /**
     * @brief View-space rotation angle in radians applied to the billboard.
     */
    float rotation {0.0f};

    /**
     * @brief Normalized anchor point inside the billboard.
     *
     * Defines the pivot used for placement and rotation of the billboard.
     *
     * - $(0.0, 0.0)$ lower-left corner of the billboard.
     * - $(0.5, 0.5)$ center of the billboard (default).
     * - $(1.0, 1.0)$ upper-right corner of the billboard.
     *
     * The billboard's world-space position corresponds to this anchor point.
     * Rotation is applied around this pivot.
     */
    Vector2 anchor = Vector2 {0.5f, 0.5f};

    /**
     * @brief Constructs a billboard.
     *
     * If material is `null` a new instance of @ref BillboardMaterial will be created.
     *
     * @param material Material used to render the billboard.
     */
    explicit Billboard(std::shared_ptr<BillboardMaterial> material);

    /**
     * @brief Creates an instance of @ref Billboard.
     *
     * @param material Shared billboard material.
     */
    [[nodiscard]] static auto
    Create(std::shared_ptr<BillboardMaterial> material = nullptr) -> std::unique_ptr<Billboard> {
        return std::make_unique<Billboard>(material);
    }

    /**
     * @brief Identifies this node as @return Node::Type::Billboard
     */
    [[nodiscard]] auto GetNodeType() const -> Node::Type override {
        return Node::Type::Billboard;
    }

    /**
     * @brief Returns the material associated with the billboard.
     */
    [[nodiscard]] auto GetMaterial() const -> std::shared_ptr<Material> override {
        return material_;
    }

    /**
     * @brief Sets the material used to render the billboard.
     *
     * @param material Shared pointer to a billboard material.
     */
    auto SetMaterial(std::shared_ptr<BillboardMaterial> material) {
        material_ = material;
    }

    /// @cond INTERNAL
    [[nodiscard]] auto GetGeometry() const -> std::shared_ptr<Geometry> override {
        return SharedGeometry();
    }
    /// @endcond

private:
    /// @cond INTERNAL
    std::shared_ptr<Material> material_;

    static auto SharedGeometry() -> std::shared_ptr<Geometry>&;
    /// @endcond
};

}
