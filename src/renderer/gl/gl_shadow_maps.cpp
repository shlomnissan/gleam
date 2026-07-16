/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/cameras/camera.hpp"
#include "vglx/cameras/orthographic_camera.hpp"
#include "vglx/cameras/perspective_camera.hpp"
#include "vglx/lights/directional_light.hpp"
#include "vglx/lights/light.hpp"
#include "vglx/lights/spot_light.hpp"
#include "vglx/math/vector3.hpp"

#include "renderer/gl/gl_program.hpp"
#include "renderer/gl/gl_shadow_maps.hpp"

#include "core/shader_library.hpp"

#include "shaders/internal/headers/shadow_depth_vert.h"
#include "shaders/internal/headers/shadow_depth_instanced_vert.h"
#include "shaders/internal/headers/shadow_depth_frag.h"

#include <algorithm>

#include <glad/glad.h>

namespace vglx {

namespace {

constexpr float border[] = {1.0f, 1.0f, 1.0f, 1.0f};

auto allocate_texture_2d_array(int count, unsigned int max_map_size) -> std::expected<GLuint, std::string> {
    GLuint texture_id {0};

    glGenTextures(1, &texture_id);
    if (texture_id == 0) {
        return std::unexpected("Failed to generate shadow map texture");
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_id);
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,
        GL_DEPTH_COMPONENT24,
        max_map_size,
        max_map_size,
        count,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, border);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    return texture_id;
}

auto update_camera(Light* light, Camera* camera) -> void {
    if (light->GetType() == Light::Type::Spot) {
        auto spot = static_cast<SpotLight*>(light);
        auto far = spot->range > 0.0f ? spot->range : spot->shadow.far;
        auto spot_camera = static_cast<PerspectiveCamera*>(camera);

        spot_camera->SetLens(spot->angle * 2.0f, spot->shadow.near, far);

        const auto target = spot->target != nullptr
            ? spot->target->GetWorldPosition()
            : Vector3::Zero();

        spot_camera->transform.SetPosition(spot->GetWorldPosition());
        spot_camera->LookAt(target);
        spot_camera->UpdateViewMatrix();
    }

    if (light->GetType() == Light::Type::Directional) {
        auto directional = static_cast<DirectionalLight*>(light);
        auto dir_camera = static_cast<OrthographicCamera*>(camera);
        auto& shadow = directional->shadow;

        dir_camera->SetLens({
            .left = -shadow.extent,
            .right = shadow.extent,
            .top = shadow.extent,
            .bottom = -shadow.extent,
            .near = shadow.near,
            .far = shadow.far
        });

        const auto target = directional->target != nullptr
            ? directional->target->GetWorldPosition()
            : Vector3::Zero();

        dir_camera->transform.SetPosition(directional->GetWorldPosition());
        dir_camera->LookAt(target);
        dir_camera->UpdateViewMatrix();
    }
}

auto create_camera(Light* light) -> std::unique_ptr<Camera> {
    switch(light->GetType()) {
        case Light::Type::Spot:
            return PerspectiveCamera::Create();
        case Light::Type::Directional:
            return OrthographicCamera::Create();
        default: return nullptr;
    }
}

}

auto GLShadowMaps::Initialize() -> std::expected<void, std::string> {
    prg_shadow_map_ = std::make_unique<GLProgram>(std::vector<ShaderInfo> {
        {.type = ShaderType::kVertexShader, .source = _SHADER_shadow_depth_vert},
        {.type = ShaderType::kFragmentShader, .source = _SHADER_shadow_depth_frag}
    });

    if (!prg_shadow_map_->IsValid()) {
        return std::unexpected("Unable to create shadow map program");
    }

    prg_instanced_shadow_map_ = std::make_unique<GLProgram>(std::vector<ShaderInfo> {
        {.type = ShaderType::kVertexShader, .source = _SHADER_shadow_depth_instanced_vert},
        {.type = ShaderType::kFragmentShader, .source = _SHADER_shadow_depth_frag}
    });

    if (!prg_instanced_shadow_map_->IsValid()) {
        return std::unexpected("Unable to create instanced shadow map program");
    }

    glGenFramebuffers(1, &buffer_id_);
    if (buffer_id_ == 0) {
        return std::unexpected("Failed to generate shadow map framebuffer");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, buffer_id_);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return {};
}

auto GLShadowMaps::StartFrame(
    unsigned int count_2d,
    unsigned int max_map_size_2d
) -> std::expected<void, std::string> {
    if (count_2d != state_2d_.count || max_map_size_2d != state_2d_.max_map_size) {
        if (state_2d_.texture_id != 0) {
            glDeleteTextures(1, &state_2d_.texture_id);
            state_2d_.texture_id = 0;
            state_2d_.count = 0;
        }

        auto result = allocate_texture_2d_array(count_2d, max_map_size_2d);
        if (!result.has_value()) {
            return std::unexpected(result.error());
        }

        state_2d_.texture_id = result.value();
        state_2d_.count = count_2d;
        state_2d_.max_map_size = max_map_size_2d;
    }

    state_2d_.curr_layer_id = 0;
    state_cube_.curr_layer_id = 0;

    for (auto& [_, shadow_map] : shadow_maps_) {
        shadow_map.touched = false;
    }

    return {};
}

auto GLShadowMaps::BindShadowMap(Light* light, Camera* camera) -> std::expected<Camera*, std::string> {
    auto config = light->GetShadow();
    if (config == nullptr) {
        return std::unexpected("Failed to read shadow config from light source");
    }

    auto it = std::ranges::find_if(shadow_maps_,
        [&light](Light* key) { return key == light; },
        &std::pair<Light*, GLShadowMap>::first
    );

    if (it == shadow_maps_.end()) {
        auto entry = GLShadowMap {};
        entry.camera = create_camera(light);
        if (entry.camera == nullptr) {
            return std::unexpected("Shadow mapping is not supported for this light type");
        }
        it = shadow_maps_.emplace(shadow_maps_.end(), light, std::move(entry));
    }

    auto& entry = it->second;
    update_camera(light, entry.camera.get());

    glBindFramebuffer(GL_FRAMEBUFFER, buffer_id_);
    glFramebufferTextureLayer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        state_2d_.texture_id,
        0,
        state_2d_.curr_layer_id
    );

    entry.touched = true;
    entry.map_idx = state_2d_.curr_layer_id++;

    // Maps view-space positions to shadow map coordinates: light clip space
    // via the light's view-projection, then a [-1,1] → [0,1] remap whose XY
    // is scaled by map_size / max_map_size_ so lookups address this light's
    // sub-viewport region within the full-size layer.

    const auto scale = 0.5f
        * static_cast<float>(config->map_size)
        / static_cast<float>(state_2d_.max_map_size);

    const auto remap = Matrix4 {
        scale, 0.0f,  0.0f, scale,
        0.0f,  scale, 0.0f, scale,
        0.0f,  0.0f,  0.5f, 0.5f,
        0.0f,  0.0f,  0.0f, 1.0f
    };

    entry.transform = remap
        * entry.camera->projection_matrix
        * entry.camera->view_matrix
        * camera->GetWorldTransform();

#if !defined(NDEBUG)
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return std::unexpected("Shadow map framebuffer is incomplete");
    }
#endif

    return entry.camera.get();
}

auto GLShadowMaps::GetShadowMap(Light* light) -> GLShadowMap* {
    auto it = std::ranges::find_if(shadow_maps_,
        [&light](Light* key) { return key == light; },
        &std::pair<Light*, GLShadowMap>::first
    );

    if (it == shadow_maps_.end()) {
        return nullptr;
    }

    return &it->second;
}

auto GLShadowMaps::GetProgram(bool instanced) -> GLProgram* {
    return instanced ? prg_instanced_shadow_map_.get() : prg_shadow_map_.get();
}

auto GLShadowMaps::EndFrame() -> void {
    std::erase_if(shadow_maps_, [](const auto& entry) {
        return !entry.second.touched;
    });
}

GLShadowMaps::~GLShadowMaps() {
    if (buffer_id_ != 0) {
        glDeleteFramebuffers(1, &buffer_id_);
        buffer_id_ = 0;
    }

    if (state_2d_.texture_id != 0) {
        glDeleteTextures(1, &state_2d_.texture_id);
        state_2d_.texture_id = 0;
    }

    if (state_cube_.texture_id != 0) {
        glDeleteTextures(1, &state_cube_.texture_id);
        state_cube_.texture_id = 0;
    }
}

}
