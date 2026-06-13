/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/scene/fog.hpp"
#include "vglx/scene/node.hpp"
#include "vglx/textures/texture.hpp"

#include <memory>
#include <optional>

namespace vglx {

/**
 * @brief Root node of a renderable scene graph.
 *
 * Scene is the top-level container for all nodes that participate in
 * rendering and updates. It owns the scene graph hierarchy and optional
 * global fog settings. Construct a scene directly with @ref Scene::Create
 * or by subclassing, attach nodes to it, and call @ref Advance once per
 * frame to drive its updates.
 *
 * @code
 * auto scene = vglx::Scene::Create();
 * scene->fog = vglx::Fog::CreateExponential(0x444444, 0.3f);
 *
 * scene->Add(vglx::Mesh::Create(
 *   vglx::BoxGeometry::Create(),
 *   vglx::PhongMaterial::Create({.color = 0x049EF4})
 * ));
 *
 * // Inside the main loop:
 * scene->Advance(delta);
 * renderer.Render(scene.get(), camera.get());
 * @endcode
 *
 * @ingroup SceneGroup
 */
class VGLX_EXPORT Scene : public Node {
public:
    /// @brief Optional global fog settings applied during rendering.
    std::optional<Fog> fog;

    /// @brief Optional for defining a flat texture background.
    std::shared_ptr<Texture> background {nullptr};

    /**
     * @brief Optional environment map used as the image-based lighting source.
     */
    std::shared_ptr<Texture> environment {nullptr};

    /// @brief Scalar multiplier applied to the @ref environment lighting contribution.
    float environment_intensity {1.0f};

    /**
     * @brief Constructs a scene object.
     */
    Scene();

    /**
     * @brief Creates an instance of @ref Scene.
     */
    [[nodiscard]] static auto Create() -> std::unique_ptr<Scene> {
        return std::make_unique<Scene>();
    }

    /**
     * @brief Advances the scene by one frame.
     *
     * Propagates per-frame updates through the scene graph, calling
     * @ref Node::OnUpdate "OnUpdate" on the scene and all attached nodes in
     * depth-first order. Call this once per frame from your main loop.
     *
     * @param delta Elapsed time in seconds since the last frame.
     */
    auto Advance(float delta) -> void;

    /**
     * @brief Identifies this node as @ref Node::Type "Node::Type::Scene".
     */
    [[nodiscard]] auto GetNodeType() const -> Node::Type override {
        return Node::Type::Scene;
    }

    ~Scene() override;

private:
    /// @cond INTERNAL
    class Impl;
    std::unique_ptr<Impl> impl_;
    /// @endcond
};

}