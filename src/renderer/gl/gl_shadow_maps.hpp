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

class GLProgram;
class Light;
class Camera;

struct GLShadowMap {
    unsigned int map_idx;

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

    auto GetProgram(bool instanced) -> GLProgram*;

    auto GetTextureId() const -> unsigned int;

    [[nodiscard]] auto StartFrame(int count, unsigned int max_map_size) -> std::expected<void, std::string>;

    [[nodiscard]] auto BindShadowMap(Light* light) -> std::expected<Camera*, std::string>;

    auto EndFrame() -> void;

    ~GLShadowMaps();

private:
    std::vector<std::pair<Light*, GLShadowMap>> shadow_maps_ {};

    std::unique_ptr<GLProgram> prg_shadow_map_;
    std::unique_ptr<GLProgram> prg_instanced_shadow_map_;

    unsigned int curr_idx_ {0};
    unsigned int max_map_size_ {0u};
    unsigned int texture_id_ {0};
    unsigned int buffer_id_ {0};

    int count_ {0};
};

}
