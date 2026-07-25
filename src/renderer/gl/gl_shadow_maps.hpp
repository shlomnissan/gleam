/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/math/matrix4.hpp"

#include <expected>
#include <memory>
#include <string>
#include <vector>

namespace vglx {

class Light;
class Camera;

struct GLShadowMap {
    unsigned int map_idx;

    Matrix4 transform {Matrix4::Identity()};

    std::unique_ptr<Camera> camera;

    bool touched {false};
};

class GLShadowMaps {
public:
    GLShadowMaps() = default;

    // Non-copyable
    GLShadowMaps(const GLShadowMaps&) = delete;
    auto operator=(const GLShadowMaps&) -> GLShadowMaps& = delete;

    // Non-moveable
    GLShadowMaps(GLShadowMaps&&) = delete;
    auto operator=(GLShadowMaps&&) -> GLShadowMaps& = delete;

    auto Initialize() -> std::expected<void, std::string>;

    [[nodiscard]] auto StartFrame(
        unsigned int count_2d,
        unsigned int max_map_size_2d,
        unsigned int count_point,
        unsigned int max_map_size_point
    ) -> std::expected<void, std::string>;

    [[nodiscard]] auto BindShadowMap(
        Light* light,
        Camera* camera,
        unsigned int face = 0
    ) -> std::expected<Camera*, std::string>;

    [[nodiscard]] auto GetShadowMap(Light* light) -> GLShadowMap*;

    [[nodiscard]] auto GetTexture2D() const -> unsigned int {
        return state_2d_.texture_id;
    }

    [[nodiscard]] auto GetPointTexture() const -> unsigned int {
        return state_point_.texture_id;
    }

    auto EndFrame() -> void;

    auto Clear() -> void;

    ~GLShadowMaps();

private:
    struct InternalShadowMapState {
        unsigned int curr_layer_id {0u};
        unsigned int max_map_size {0u};
        unsigned int texture_id {0u};
        unsigned int count {0u};
    };

    std::vector<std::pair<Light*, GLShadowMap>> shadow_maps_ {};

    InternalShadowMapState state_2d_ {};
    InternalShadowMapState state_point_ {};

    unsigned int buffer_id_ {0};
};

}
