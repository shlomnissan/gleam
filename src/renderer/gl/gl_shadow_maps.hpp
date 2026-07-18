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

class GLProgram;
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
        unsigned int count_cube,
        unsigned int max_map_size_cube
    ) -> std::expected<void, std::string>;

    [[nodiscard]] auto BindShadowMap(Light* light, Camera* camera) -> std::expected<Camera*, std::string>;

    [[nodiscard]] auto GetShadowMap(Light* light) -> GLShadowMap*;

    [[nodiscard]] auto GetProgram(bool instanced) -> GLProgram*;

    [[nodiscard]] auto GetTexture2D() const -> unsigned int {
        return state_2d_.texture_id;
    }

    [[nodiscard]] auto GetTextureCube() const -> unsigned int {
        return state_cube_.texture_id;
    }

    auto EndFrame() -> void;

    ~GLShadowMaps();

private:
    struct InternalShadowMapState {
        unsigned int curr_layer_id {0u};
        unsigned int max_map_size {0u};
        unsigned int texture_id {0u};
        unsigned int count {0u};
    };

    std::vector<std::pair<Light*, GLShadowMap>> shadow_maps_ {};

    std::unique_ptr<GLProgram> prg_shadow_map_;
    std::unique_ptr<GLProgram> prg_instanced_shadow_map_;

    InternalShadowMapState state_2d_ {};
    InternalShadowMapState state_cube_ {};

    unsigned int buffer_id_ {0};
};

}
