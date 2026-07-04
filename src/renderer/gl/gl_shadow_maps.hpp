/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <expected>
#include <memory>
#include <string>
#include <vector>

namespace vglx {

class Light;
class Camera;

struct GLShadowMap {
    unsigned int buffer_id;
    unsigned int texture_id;
    unsigned int map_size;

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

    auto StartFrame() -> void;

    [[nodiscard]] auto BindShadowMap(Light* light) -> std::expected<Camera*, std::string>;

    auto EndFrame() -> void;

    ~GLShadowMaps();

private:
    std::vector<std::pair<Light*, GLShadowMap>> shadow_maps_ {};
};

}