/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/loaders.hpp"

#include <memory>

namespace vglx {

class Camera;
class Window;
class LoadScheduler;

/**
 * @brief Provides access to shared runtime parameters and services.
 *
 * The shared context encapsulates common runtime state and services accessible
 * throughout the scene graph. It is constructed and propagated automatically
 * by the runtime, which also keeps its values synchronized as rendering
 * parameters change, such as window size, framebuffer dimensions, or the
 * active camera.
 *
 * Nodes can access the shared context via the @ref Node::OnAttached
 * callback, which is invoked when the node joins an active scene hierarchy.
 * Override this method in your node subclass to perform initialization
 * that depends on the context, such as loading resources, querying the
 * active camera, or accessing other global services.
 *
 * @code
 * auto MyNode::OnAttached(SharedContextPointer context) -> void {
 *   // It's common for the scene to use the OnAttached hook to
 *   // configure the camera position
 *   context->camera->TranslateZ(3.0f);
 * }
 * @endcode
 *
 * @ingroup CoreGroup
 */
struct VGLX_EXPORT SharedContext {
    /**
     * @brief Pointer to the active camera.
     *
     * Commonly used by scene nodes to query camera parameters or implement
     * camera controls.
     */
    Camera* camera;

    /**
     * @brief Aspect ratio of the render surface.
     *
     * Computed as framebuffer width divided by height. Used for projection and
     * viewport setup.
     */
    float aspect_ratio;

    /**
     * @brief Framebuffer width in physical pixels.
     *
     * Reflects the true pixel width of the render target. May differ from
     * window width on high-DPI displays.
     */
    int framebuffer_width;

    /**
     * @brief Framebuffer height in physical pixels.
     *
     * Reflects the true pixel height of the render target.
     */
    int framebuffer_height;

    /**
     * @brief Window width in logical units.
     *
     * Represents window dimensions in OS coordinate space. Differs from
     * framebuffer size on high-DPI displays.
     */
    int window_width;

    /**
     * @brief Window height in logical units.
     *
     * Represents window dimensions in OS coordinate space.
     */
    int window_height;

    /**
     * @brief Texture loader.
     *
     * Handles loading and caching of image assets targeting the engine's
     * custom `.tex` format.
     */
    std::unique_ptr<TextureLoader> texture_loader;

    /**
     * @brief Mesh loader.
     *
     * Handles loading and caching of mesh assets targeting the engine's
     * custom `.msh` format.
     */
    std::unique_ptr<MeshLoader> mesh_loader;

    /**
     * @brief Creates and initializes a shared runtime context.
     *
     * Creates an instance of @ref SharedContext populated with the current
     * window and framebuffer dimensions, active camera reference, and
     * shared loader services. This factory is intended to be called by the
     * runtime during initialization.
     *
     * @param window Pointer to the active window.
     * @param camera Pointer to the active camera.
     * @param scheduler Load scheduler used by resource loaders for async loading.
     */
    [[nodiscard]] static auto Create(
        const Window* window,
        Camera* camera,
        LoadScheduler* scheduler
    ) -> std::unique_ptr<SharedContext>;
};

/**
 * @brief Alias for a unique pointer to a @ref SharedContext instance.
 *
 * This typedef simplifies ownership semantics for the shared context,
 * ensuring a single owner at any given time. Use this when returning
 * or storing the main rendering context instance.
 *
 * @related SharedContext
 */
using SharedContextPointer = const vglx::SharedContext*;

}