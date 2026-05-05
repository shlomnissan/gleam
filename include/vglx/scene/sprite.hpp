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
#include "vglx/materials/sprite_material.hpp"
#include "vglx/math/vector2.hpp"
#include "vglx/scene/renderable.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Billboarded quad that always faces the active camera.
 *
 * Sprite is a flat unit-sized quad oriented toward the camera at all times.
 * It is usually textured and exposes the minimal Renderable interface so it
 * can be submitted to the renderer. Common use cases include particles,
 * labels, icons, and world-space markers.
 *
 * @code
 * MyNode::MyNode() {
 *   auto texture = LoadTexture("assets/sprite.png");
 *   if (texture.has_value()) {
 *     auto material = vglx::SpriteMaterial::Create(texture.value());
 *     Add(vglx::Sprite::Create(material))->SetScale(0.5f);
 *   } else {
 *     std::println(stderr, "{}", texture.error());
 *   }
 * }
 * @endcode
 *
 * @ingroup SceneGroup
 */
class VGLX_EXPORT Sprite : public Renderable {
public:
    /**
     * @brief View-space rotation angle in radians applied to the sprite.
     */
    float rotation {0.0f};

    /**
     * @brief Normalized anchor point inside the sprite.
     *
     * Defines the pivot used for placement and rotation of the sprite.
     *
     * - $(0.0, 0.0)$ lower-left corner of the sprite.
     * - $(0.5, 0.5)$ center of the sprite (default).
     * - $(1.0, 1.0)$ upper-right corner of the sprite.
     *
     * The sprite's world-space position corresponds to this anchor point.
     * Rotation is applied around this pivot.
     */
    Vector2 anchor = Vector2 {0.5f, 0.5f};

    /**
     * @brief Constructs a sprite.
     *
     * If material is `null` a new instance of @ref SpriteMaterial will be created.
     *
     * @param material Material used to render the sprite.
     */
    Sprite(std::shared_ptr<SpriteMaterial> material);

    /**
     * @brief Creates an instance of @ref Sprite.
     *
     * @param material Shared sprite material.
     */
    [[nodiscard]] static auto
    Create(std::shared_ptr<SpriteMaterial> material = nullptr) -> std::unique_ptr<Sprite> {
        return std::make_unique<Sprite>(material);
    }

    /**
     * @brief Identifies this node as @return Node::Type::Sprite
     */
    [[nodiscard]] auto GetNodeType() const -> Node::Type override {
        return Node::Type::Sprite;
    }

    /**
     * @brief Returns the material associated with the sprite.
     */
    [[nodiscard]] auto GetMaterial() const -> std::shared_ptr<Material> override {
        return material_;
    }

    /**
     * @brief Sets the material used to render the sprite.
     *
     * @param material Shared pointer to a sprite material.
     */
    auto SetMaterial(std::shared_ptr<SpriteMaterial> material) {
        material_ = material;
    }

    /// @cond INTERNAL
    [[nodiscard]] auto GetGeometry() const -> std::shared_ptr<Geometry> override {
        return geometry_;
    }
    /// @endcond

private:
    /// @cond INTERNAL
    std::shared_ptr<Geometry> geometry_;
    std::shared_ptr<Material> material_;

    static auto SharedGeometry() -> std::shared_ptr<Geometry>&;
    /// @endcond
};

}