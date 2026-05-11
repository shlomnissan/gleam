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
 * global fog settings. Create a scene by overriding the application
 * runtime @ref Application::CreateScene. The runtime will advance it
 * once per frame.
 *
 * @code
 * class MyApp : public vglx::Application {
 * public:
 *   auto Configure() -> Application::Parameters override {
 *     return {
 *       .title = "Hello VGLX",
 *       .clear_color = {0x000000},
 *       .width = 1280,
 *       .height = 720,
 *       .antialiasing = 4,
 *     };
 *   }
 *
 *   auto CreateScene() -> std::unique_ptr<vglx::Scene> override {
 *     auto scene = vglx::Scene::Create();
 *     scene->fog = vglx::Fog::CreateExponential(0x444444, 0.3f);
 *
 *     // Add nodes to the scene...
 *
 *     return scene;
 *   }
 *
 *   auto Update(float delta) -> bool override {
 *     return true;
 *   }
 * };
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
     * depth-first order. This is invoked automatically by the runtime once
     * per frame.
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