/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/core/renderer.hpp"
#include "vglx/scene/renderable.hpp"

#include "renderer/gl/gl_buffers.hpp"
#include "renderer/gl/gl_camera.hpp"
#include "renderer/gl/gl_lights.hpp"
#include "renderer/gl/gl_programs.hpp"
#include "renderer/gl/gl_state.hpp"
#include "renderer/gl/gl_textures.hpp"

#include <memory>

namespace vglx {

class RenderLists;

class Renderer::Impl {
public:
    explicit Impl(const Renderer::Parameters& params);

    Impl(const Impl&) = delete;
    Impl(Impl&&) = delete;
    Impl& operator=(const Impl&) = delete;
    Impl& operator=(Impl&&) = delete;

    [[nodiscard]] auto Initialize() -> std::expected<void, std::string>;

    auto Render(Scene* scene, Camera* camera) -> void;

    auto SetViewport(int x, int y, int width, int height) -> void;

    auto SetClearColor(const Color& color) -> void;

    [[nodiscard]] auto RenderedObjectsPerFrame() const {
        return rendered_objects_per_frame_;
    }

    ~Impl();

private:
    GLBuffers buffers_;
    GLCamera camera_ubo_;
    GLLights lights_;
    GLPrograms programs_;
    GLState state_;
    GLTextures textures_;

    Renderer::Parameters params_;

    std::unique_ptr<RenderLists> render_lists_;

    size_t rendered_objects_counter_ {0};
    size_t rendered_objects_per_frame_ {0};

    auto ProcessLights(Camera* camera) -> void;

    auto RenderObjects(Scene* scene, Camera* camera) -> void;

    auto RenderObject(Renderable* renderable, Scene* scene, Camera* camera) -> void;

    auto SetUniforms(
        GLProgram* program,
        ProgramAttributes* attrs,
        Renderable* renderable,
        Camera* camera,
        Scene* scene
    ) -> void;
};

}