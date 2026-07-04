/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/cameras/camera.hpp"
#include "vglx/cameras/perspective_camera.hpp"
#include "vglx/lights/light.hpp"
#include "vglx/lights/spot_light.hpp"
#include "vglx/math/vector3.hpp"

#include "renderer/gl/gl_shadow_maps.hpp"

#include <algorithm>

#include <glad/glad.h>

namespace vglx {

namespace {

auto allocate_resources(GLShadowMap& shadow_map) -> std::expected<void, std::string> {
    // TODO: implement
    return {};
}

auto deallocate_resources(GLShadowMap& shadow_map) {
    if (shadow_map.buffer_id != 0) glDeleteFramebuffers(1, &shadow_map.buffer_id);
    if (shadow_map.texture_id != 0) glDeleteTextures(1, &shadow_map.texture_id);

    shadow_map.buffer_id = 0;
    shadow_map.texture_id = 0;
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
}

auto create_camera(Light* light) -> std::unique_ptr<Camera> {
    if (light->GetType() == Light::Type::Spot) {
        auto spot = static_cast<SpotLight*>(light);
        return PerspectiveCamera::Create({
            .fov = spot->angle * 2.0f,
            .aspect = 1.0f,
            .near = spot->shadow.near,
            .far = spot->range > 0.0f ? spot->range : spot->shadow.far
        });
    }
    return nullptr;
}

}

auto GLShadowMaps::StartFrame() -> void {
    for (auto& [_, shadow_map] : shadow_maps_) {
        shadow_map.touched = false;
    }
}

auto GLShadowMaps::BindShadowMap(Light* light) -> std::expected<Camera*, std::string> {
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

    if (entry.map_size != config->map_size || entry.buffer_id == 0) {
        entry.map_size = config->map_size;
        deallocate_resources(entry);
        if (auto res = allocate_resources(entry); !res.has_value()) {
            return std::unexpected(res.error());
        }
    }

    entry.touched = true;

    glBindFramebuffer(GL_FRAMEBUFFER, entry.buffer_id);
    return entry.camera.get();
}

auto GLShadowMaps::EndFrame() -> void {
    for (auto& [_, entry] : shadow_maps_) {
        if (!entry.touched) deallocate_resources(entry);
    }

    std::erase_if(shadow_maps_, [](const auto& entry) {
        return !entry.second.touched;
    });
}

GLShadowMaps::~GLShadowMaps() {
    for (auto& [_, entry] : shadow_maps_) {
        deallocate_resources(entry);
    }
}

}