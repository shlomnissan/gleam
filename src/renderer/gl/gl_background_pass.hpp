/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/textures/texture.hpp"

#include "renderer/gl/gl_program.hpp"

#include <expected>
#include <memory>
#include <string>

#include <glad/glad.h>

namespace vglx {

class GLBackgroundPass {
public:
    GLBackgroundPass() = default;

    // Non-copyable
    GLBackgroundPass(const GLBackgroundPass&) = delete;
    auto operator=(const GLBackgroundPass&) -> GLBackgroundPass& = delete;

    // Non-moveable
    GLBackgroundPass(GLBackgroundPass&&) = delete;
    auto operator=(const GLBackgroundPass&&) -> GLBackgroundPass& = delete;

    [[nodiscard]] auto Initialize() -> std::expected<void, std::string>;

    auto Render(const std::shared_ptr<Texture>& background) const -> void;

    ~GLBackgroundPass();

private:
    GLuint vao_ {0};

    std::unique_ptr<GLProgram> program_ {nullptr};
};

}