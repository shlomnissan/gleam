/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/core/renderer.hpp"
#include "vglx/scene/renderable.hpp"

#include "renderer/gl/gl_background_pass.hpp"
#include "renderer/gl/gl_camera.hpp"
#include "renderer/gl/gl_device.hpp"
#include "renderer/gl/gl_environment.hpp"
#include "renderer/gl/gl_framebuffers.hpp"
#include "renderer/gl/gl_lights.hpp"
#include "renderer/gl/gl_present_pass.hpp"
#include "renderer/gl/gl_programs.hpp"
#include "renderer/gl/gl_scene_buffer.hpp"
#include "renderer/gl/gl_shadow_maps.hpp"
#include "renderer/gl/gl_state.hpp"
#include "renderer/gl/gl_textures.hpp"
#include "renderer/gl/gl_vertex_buffers.hpp"

#include <memory>

namespace vglx {

class RenderLists;
class RenderTarget;

class Renderer::Impl {
public:
    explicit Impl(const Renderer::Parameters& params);

    Impl(const Impl&) = delete;
    Impl(Impl&&) = delete;
    Impl& operator=(const Impl&) = delete;
    Impl& operator=(Impl&&) = delete;

    [[nodiscard]] auto Initialize() -> std::expected<void, std::string>;

    auto Render(Scene* scene, Camera* camera, RenderTarget* target = nullptr) -> void;

    auto SetViewport(int x, int y, int width, int height) -> void;

    auto SetClearColor(const Color& color) -> void;

    auto SetToneMapping(ToneMapping tone_mapping) -> void;

    auto SetExposure(float exposure) -> void;

    auto CreateTextureFromRenderTarget(RenderTarget* target) -> std::shared_ptr<Texture2D>;

    [[nodiscard]] auto RenderedObjectsPerFrame() const {
        return rendered_objects_per_frame_;
    }

    [[nodiscard]] auto GetLimits() const -> const Renderer::Limits& {
        return gl::limits();
    }

    [[nodiscard]] auto GetDriverInfo() const -> const Renderer::DriverInfo& {
        return gl::driver_info();
    }

    ~Impl();

private:
    GLBackgroundPass background_pass_;
    GLCamera camera_ubo_;
    GLEnvironment environment_;
    GLFramebuffers framebuffers_;
    GLLights lights_;
    GLPresentPass present_pass_;
    GLPrograms programs_;
    GLSceneBuffer scene_buffer_;
    GLShadowMaps shadow_maps_;
    GLState state_;
    GLTextures textures_;
    GLVertexBuffers vertex_buffers_;

    Renderer::Parameters params_;

    std::unique_ptr<RenderLists> render_lists_;
    std::unique_ptr<RenderLists> shadow_render_lists_;

    size_t rendered_objects_counter_ {0};
    size_t rendered_objects_per_frame_ {0};

    Renderer::ShadowMap shadow_map_ {Renderer::ShadowMap::None};

    Renderer::ToneMapping tone_mapping_ {Renderer::ToneMapping::None};

    GLEnvironmentMaps env_maps_ {};

    float exposure_ {1.0f};

    int next_texture_unit_ {0};

    auto ProcessLights(Camera* camera) -> void;

    auto RenderObjects(Scene* scene, Camera* camera) -> void;

    auto RenderObject(Renderable* renderable, Scene* scene, Camera* camera) -> void;

    auto RenderShadowMaps(Scene* scene, Camera* camera) -> void;

    auto SetUniforms(
        GLProgram* program,
        ProgramAttributes* attrs,
        Renderable* renderable,
        Camera* camera,
        Scene* scene
    ) -> void;
};

}