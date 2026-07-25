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
#include "vglx/lights/point_light.hpp"
#include "vglx/lights/spot_light.hpp"
#include "vglx/math/vector3.hpp"

#include "renderer/gl/gl_shadow_maps.hpp"

#include <algorithm>

#include <glad/glad.h>

namespace vglx {

namespace {

constexpr float border[] = {1.0f, 1.0f, 1.0f, 1.0f};

auto allocate_texture(unsigned int count, unsigned int max_map_size, GLenum type) -> std::expected<GLuint, std::string> {
    GLuint texture_id {0};

    glGenTextures(1, &texture_id);
    if (texture_id == 0) {
        return std::unexpected("Failed to generate shadow map texture");
    }

    auto sides = type == GL_TEXTURE_CUBE_MAP_ARRAY ? 6 : 1;

    glBindTexture(type, texture_id);
    glTexImage3D(
        type,
        0,
        GL_DEPTH_COMPONENT24,
        max_map_size,
        max_map_size,
        count * sides,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        nullptr
    );

    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (type == GL_TEXTURE_2D_ARRAY) {
        glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterfv(type, GL_TEXTURE_BORDER_COLOR, border);
    }

    glTexParameteri(type, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(type, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glBindTexture(type, 0);

    return texture_id;
}

auto update_camera(Light* light, Camera* camera) -> void {
    if (light->GetType() == Light::Type::Point) {
        auto point = static_cast<PointLight*>(light);
        auto far = point->range > 0.0f ? point->range : point->shadow.far;
        auto point_camera = static_cast<PerspectiveCamera*>(camera);

        point_camera->SetLens(math::DegToRad(90.0f), point->shadow.near, far);

        point_camera->transform.SetPosition(point->GetWorldPosition());
    }

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
        case Light::Type::Point:
        case Light::Type::Spot:
            return PerspectiveCamera::Create();
        case Light::Type::Directional:
            return OrthographicCamera::Create();
        default: return nullptr;
    }
}

auto set_camera_face(PerspectiveCamera* camera, unsigned int face) -> void {
    auto direction = Vector3::Zero();
    auto up = Vector3::Zero();

    switch (face) {
        case 0: direction = { 1.0f,  0.0f,  0.0f}; up = {0.0f, -1.0f,  0.0f}; break;
        case 1: direction = {-1.0f,  0.0f,  0.0f}; up = {0.0f, -1.0f,  0.0f}; break;
        case 2: direction = { 0.0f,  1.0f,  0.0f}; up = {0.0f,  0.0f,  1.0f}; break;
        case 3: direction = { 0.0f, -1.0f,  0.0f}; up = {0.0f,  0.0f, -1.0f}; break;
        case 4: direction = { 0.0f,  0.0f,  1.0f}; up = {0.0f, -1.0f,  0.0f}; break;
        case 5: direction = { 0.0f,  0.0f, -1.0f}; up = {0.0f, -1.0f,  0.0f}; break;
    }

    camera->up = up;
    camera->LookAt(camera->GetWorldPosition() + direction);
    camera->UpdateViewMatrix();
}

}

auto GLShadowMaps::Initialize() -> std::expected<void, std::string> {
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
    unsigned int max_map_size_2d,
    unsigned int count_point,
    unsigned int max_map_size_point
) -> std::expected<void, std::string> {
    state_2d_.curr_layer_id = 0;
    state_point_.curr_layer_id = 0;

    for (auto& [_, shadow_map] : shadow_maps_) {
        shadow_map.touched = false;
    }

    if (count_2d != state_2d_.count || max_map_size_2d != state_2d_.max_map_size) {
        if (state_2d_.texture_id != 0) {
            glDeleteTextures(1, &state_2d_.texture_id);
            state_2d_.texture_id = 0;
            state_2d_.count = 0;
        }

        if (count_2d > 0) {
            auto result = allocate_texture(count_2d, max_map_size_2d, GL_TEXTURE_2D_ARRAY);
            if (!result.has_value()) return std::unexpected(result.error());
            state_2d_.texture_id = result.value();
            state_2d_.count = count_2d;
        }

        state_2d_.max_map_size = max_map_size_2d;
    }

    if (count_point != state_point_.count || max_map_size_point != state_point_.max_map_size) {
        if (state_point_.texture_id != 0) {
            glDeleteTextures(1, &state_point_.texture_id);
            state_point_.texture_id = 0;
            state_point_.count = 0;
        }

        if (count_point > 0) {
            auto result = allocate_texture(count_point, max_map_size_point, GL_TEXTURE_CUBE_MAP_ARRAY);
            if (!result.has_value()) return std::unexpected(result.error());
            state_point_.texture_id = result.value();
            state_point_.count = count_point;
        }

        state_point_.max_map_size = max_map_size_point;
    }

    return {};
}

auto GLShadowMaps::BindShadowMap(Light* light, Camera* camera, unsigned int face) -> std::expected<Camera*, std::string> {
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

    glBindFramebuffer(GL_FRAMEBUFFER, buffer_id_);

    if (light->GetType() == Light::Type::Point) {
        if (!entry.touched) {
            entry.touched = true;
            entry.map_idx = state_point_.curr_layer_id++;
            update_camera(light, entry.camera.get());
        }

        set_camera_face(static_cast<PerspectiveCamera*>(entry.camera.get()), face);

        glFramebufferTextureLayer(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            state_point_.texture_id,
            0,
            entry.map_idx * 6 + face
        );
    } else {
        entry.touched = true;
        entry.map_idx = state_2d_.curr_layer_id++;
        update_camera(light, entry.camera.get());

        glFramebufferTextureLayer(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            state_2d_.texture_id,
            0,
            entry.map_idx
        );

        // Maps view-space positions to shadow map coordinates: light clip space
        // via the light's view-projection, then a [-1,1] → [0,1] remap whose XY
        // is scaled by map_size / max_map_size_ so lookups address this light's
        // sub-viewport region within the full-size layer. Point lights skip
        // this: cube lookups are direction-based and have no sub-viewport,
        // so the receiver reconstructs depth from ShadowNear/ShadowFar instead.

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
    }

#if !defined(NDEBUG)
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        entry.touched = false;
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

    Clear();
}

auto GLShadowMaps::Clear() -> void {
    if (state_2d_.texture_id != 0) {
        glDeleteTextures(1, &state_2d_.texture_id);
    }

    if (state_point_.texture_id != 0) {
        glDeleteTextures(1, &state_point_.texture_id);
    }

    state_2d_ = {};
    state_point_ = {};
    shadow_maps_.clear();
}

}
