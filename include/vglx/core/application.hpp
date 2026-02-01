/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/cameras/camera.hpp"
#include "vglx/math/color.hpp"
#include "vglx/scene/scene.hpp"

#include <memory>
#include <string>

namespace vglx {

/**
 * @brief The runtime entry point for defining and launching an app.
 *
 * The runtime sets up the window, rendering context, a main loop, and calls
 * your hooks. Subclass it and override @ref Configure, @ref CreateScene, and
 * @ref Update to define behavior.
 *
 *
 * @code
 * class MyApp : public vglx::Application {
 *  public:
 *   auto Configure() -> Application::Parameters override {
 *     return {
 *       .title = "My App",
 *       .width = 1280,
 *       .height = 720,
 *       .clear_color = 0x444444,
 *       .vsync = true,
 *       .show_stats = true
 *     };
 *   }
 *
 *   auto CreateScene() -> std::unique_ptr<vglx::Scene> override {
 *     auto scene = vglx::Scene::Create();
 *     // Add nodes to the scene...
 *     return scene;
 *   }
 *
 *   // Optional: override CreateCamera() to configure your own camera.
 *   // auto CreateCamera() -> std::unique_ptr<vglx::Camera> override { ... }
 *
 *   auto Update(float delta) -> bool override {
 *     // Per-frame logic. Return false to exit the main loop.
 *     return true;
 *   }
 * };
 * @endcode
 *
 * This is the preferred way to initialize a new app. If you need
 * complete control, you can also assemble a program manually.
 *
 * @ingroup CoreGroup
 */
class VGLX_EXPORT Application {
public:
    /**
     * @brief Parameters for configuring an application object.
     *
     * These values are not passed directly to the constructor. To override
     * the defaults, implement the @ref Configure function and return an
     * instance with customized settings.
     */
    struct Parameters {
        std::string title {"Application"}; ///< Window title.
        Color clear_color {0x000000}; ///< Background clear color.
        int width {1280}; ///< Window width in pixels.
        int height {720}; ///< Window height in pixels.
        int sample_count {1}; ///< Antialiasing level (e.g., 4x MSAA).
        bool vsync {true}; ///< Enables vertical sync.
        bool show_stats {false}; ///< Show stats UI overlay.
    };

    Application();

    // Non-copyable
    Application(const Application&) = delete;
    auto operator=(const Application&) -> Application& = delete;

    // Movable
    Application(Application&&) noexcept = default;
    auto operator=(Application&&) noexcept -> Application& = default;

    /**
     * @brief Starts the application loop.
     *
     * This method initializes the window, rendering context, and user scene
     * and enters the main loop until the application exits.
     */
    auto Start() -> void;

    /**
     * @brief Provides @ref Application::Parameters "configuration parameters" for the application.
     *
     * Override this method to customize window settings, clear color,
     * antialiasing, vsync, and other runtime options before the
     * application starts.
     */
    virtual auto Configure() -> Parameters {
        return Parameters {};
    };

    /**
     * @brief Creates the root scene graph.
     *
     * This method **must be implemented** by the user and returns the primary
     * scene used for rendering and updates.
     */
    virtual auto CreateScene() -> std::unique_ptr<Scene> = 0;

    /**
     * @brief Creates the main camera.
     *
     * This method can be optionally overridden. If null is returned, a default
     * @ref PerspectiveCamera "perspective camera" will be created automatically.
     */
    virtual auto CreateCamera() -> std::unique_ptr<Camera> { return nullptr; }

    /**
     * @brief Per-frame update callback.
     *
     * This method **must be implemented** and is called every frame with the
     * elapsed time since the last frame. Return `false` to exit the main loop
     * and close the application.
     *
     * @param delta Time in seconds since the last frame.
     */
    virtual auto Update(float delta) -> bool = 0;

    /**
     * @brief Returns a pointer to current active @ref SharedContext "shared context".
     *
     * The shared context holds runtime properties (e.g., window size,
     * framebuffer size, aspect ratio, active camera) and provides access to
     * built-in resource loaders. It is created internally during application
     * startup and is guaranteed to remain valid for the lifetime of the
     * application.
     *
     * @return SharedContext::SharedContextPointer
     */
    [[nodiscard]] auto GetContext() const -> SharedContextPointer;

    /**
     * @brief Sets the active scene.
     *
     * @param scene Unique pointer to the new scene.
     */
    auto SetScene(std::unique_ptr<Scene> scene) -> void;

    /**
     * @brief Sets the active camera.
     *
     * @param camera Unique pointer to the new camera.
     */
    auto SetCamera(std::unique_ptr<Camera> camera) -> void;

    virtual ~Application();

private:
    /// @cond INTERNAL
    class Impl;
    std::unique_ptr<Impl> impl_;

    bool show_stats_ = false;

    auto Setup() -> void;
    /// @endcond
};

}