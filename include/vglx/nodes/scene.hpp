/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/nodes/fog.hpp"
#include "vglx/nodes/node.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Root node and entry point for a scene graph.
 *
 * `Scene` is the top-level container for all nodes in a VGLX application. It acts as the
 * root of the scene graph and is responsible for propagating update and input events
 * throughout the hierarchy. Each application has one active scene at a time, which is
 * managed by the `Application`.
 *
 * A custom scene can be created by inheriting from this class and overriding behavior
 * or adding initial nodes. The scene must be provided to the application during startup:
 *
 * @code
 * class MyApp : public vglx::Application {
 * public:
 *   auto Configure() -> void override {
 *     params.title = "My App";
 *   }
 *
 *   auto CreateScene() -> std::shared_ptr<vglx::Scene> override {
 *     auto scene = vglx::Scene::Create();
 *     // Add nodes to the scene
 *     return scene;
 *   }
 *
 *   auto Update(float delta) -> bool override {
 *     // Called every frame
 *     return true;
 *   }
 * };
 * @endcode
 *
 * @note Only one scene can be attached to the application context at a time.
 * Attaching a new scene using vglx::Application::SetScene replaces the current one.
 *
 * @ingroup NodesGroup
 */
class VGLX_EXPORT Scene : public Node {
public:
    /**
     * @brief Fog settings applied to the entire scene.
     *
     * Set this to a `LinearFog` or `ExponentialFog` instance to enable distance-based
     * atmospheric fading. This is typically done during scene setup.
     *
     * @code
     * my_scene->fog = vglx::LinearFog::Create(0x444444, 2.0f, 6.0f);
     * @endcode
     *
     * @see vglx::LinearFog
     * @see vglx::ExponentialFog
     */
    std::unique_ptr<Fog> fog;

    /**
     * @brief Constructs an Scene instance.
     */
    Scene();

    /**
     * @brief Advances the scene by one frame.
     *
     * Propagates the per-frame update through the scene graph, calling
     * `Node::Update(float delta)` on all attached nodes in depth-first order.
     * This is invoked automatically by the runtime each frame.
     *
     * @param delta Elapsed time in seconds since the last frame.
     */
    auto Advance(float delta) -> void;

    /**
     * @brief Attaches a shared context to the scene.
     * @deprecated
     *
     * The context provides runtime parameters (e.g., window size, active camera)
     * and resource loaders. This is normally called by the runtime during
     * initialization. All nodes added to the scene will receive the context
     * via `Node::OnAttached`.
     *
     * @param context Pointer to the active SharedContext instance (const).
     */
    auto SetContext(SharedContextPointer context) -> void;

    /**
     * @brief Creates a shared pointer to a Scene object.
     *
     * @return std::shared_ptr<Scene>
     */
    [[nodiscard]] static auto Create() {
        return std::make_shared<Scene>();
    }

    /**
     * @brief Returns node type.
     *
     * @return Node::Type::Scene
     */
    [[nodiscard]] auto GetNodeType() const -> Node::Type override {
        return Node::Type::Scene;
    }

    /**
     * @brief Destructor.
     */
    ~Scene() override;

private:
    /// @cond INTERNAL
    class Impl;
    std::unique_ptr<Impl> impl_;
    /// @endcond
};

}