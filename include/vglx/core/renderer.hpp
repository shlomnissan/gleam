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

class RenderTarget;

/**
 * @brief Renderer interface for drawing a scene with a given camera.
 *
 * The renderer owns GPU state and draw logic for rendering a @ref Scene with a
 * specified @ref Camera. It is typically constructed and driven by the
 * @ref Application runtime but can also be used directly in manual setups.
 *
 * This class defines the rendering interface only. The actual rendering
 * implementation is provided by a backend, and multiple backends (for example,
 * OpenGL or Vulkan) may exist behind this interface.
 *
 * @code
 * vglx::Renderer renderer({
 *   .framebuffer_width = window.FramebufferWidth(),
 *   .framebuffer_height = window.FramebufferHeight(),
 *   .clear_color = 0x444444
 * });
 *
 * auto ok = renderer.Initialize();
 * if (!ok) {
 *   HandleError(ok.error());
 * }
 * @endcode
 *
 * The renderer assumes a valid graphics context is current on the
 * calling thread. When the window is resized, call @ref SetViewport to adjust
 * the render area (or recreate with new parameters if you manage your own
 * framebuffers).
 *
 * @ingroup CoreGroup
 */
class VGLX_EXPORT Renderer {
public:
    /// @brief Parameters for constructing a @ref Renderer object.
    struct Parameters {
        int framebuffer_width {1280}; ///< Current framebuffer width in pixels.
        int framebuffer_height {720}; ///< Current framebuffer height in pixels.
        int sample_count {1}; ///< Antialiasing level (e.g., 4x MSAA).
        Color clear_color {0x000000}; ///< Clear color used at the start of a frame.
        bool reverse_z {false}; ///< Enables Reverse-Z depth mapping for improved precision.
    };

    /**
     * @brief Constructs a renderer.
     *
     * GPU resources are not created until @ref Initialize is called.
     *
     * @param params @ref Renderer::Parameters "Initialization parameters"
     * for constructing the renderer.
     */
    explicit Renderer(const Renderer::Parameters& params);

    // Non-copyable
    Renderer(const Renderer&) = delete;
    auto operator=(const Renderer&) -> Renderer& = delete;

    // Movable
    Renderer(Renderer&&) noexcept;
    auto operator=(Renderer&&) noexcept -> Renderer&;

    /**
     * @brief Initializes GPU state and allocates required resources.
     */
    [[nodiscard]] auto Initialize() -> std::expected<void, std::string>;

    /**
     * @brief Renders the given scene from the specified camera.
     *
     * The scene is expected to be in a consistent state for rendering.
     * If you are using the runtime path, this is handled automatically.
     * In direct initialization flows, call tye per-frame update routine
     * @ref Scene::Advance prior to rendering.
     *
     * @param scene Pointer to the scene to render.
     * @param camera Pointer to the active camera.
     */
    auto Render(Scene* scene, Camera* camera, RenderTarget* target = nullptr) -> void;

    /**
     * @brief Sets the active viewport rectangle in pixels.
     *
     * Adjusts the area of the framebuffer that subsequent draw calls will target.
     * This should be called whenever the window or framebuffer size changes, or
     * when rendering to a specific sub-region of the target surface.
     *
     * When using the runtime-managed rendering path, the viewport is updated
     * automatically. In manual initialization flows, you are responsible for
     * calling this method whenever the framebuffer dimensions change.
     *
     * @param x Left pixel of the viewport.
     * @param y Bottom pixel of the viewport.
     * @param width Viewport width in pixels.
     * @param height Viewport height in pixels.
     */
    auto SetViewport(int x, int y, int width, int height) -> void;

    /**
     * @brief Sets the clear color for subsequent frames.
     *
     * The color is applied at the start of each frame when the framebuffer
     * is cleared. Typically used to define the background color of the
     * rendering surface.
     *
     * @param color Clear color in RGB format.
     */
    auto SetClearColor(const Color& color) -> void;

    /**
     * @brief Returns the number of renderable objects drawn in the last frame.
     *
     * Intended for statistics overlays and debugging.
     */
    [[nodiscard]] auto RenderedObjectsPerFrame() const -> size_t;

    /**
     * @brief Returns the color texture associated with an offscreen render target.
     *
     * When rendering to a @ref RenderTarget the renderer attaches a 2D texture
     * that receives the color output of the frame. This method retrieves that
     * texture so it can be bound in subsequent passes.
     *
     * The returned pointer is shared with the render target and remains valid as
     * long as both the renderer and the render target are alive. If `target` is
     * null or does not have an associated color texture an empty pointer is returned.
     *
     * @param target Render target whose color texture should be retrieved.
     */
    [[nodiscard]] auto CreateTextureFromRenderTarget(RenderTarget* target)
        -> std::shared_ptr<Texture2D>;

    virtual ~Renderer();

private:
    /// @cond INTERNAL
    class Impl;
    std::unique_ptr<Impl> impl_;
    /// @endcond
};

}