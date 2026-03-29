/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/textures/texture.hpp"

#include "renderer/gl/gl_program.hpp"

#include <array>
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

    auto Initialize() -> std::expected<void, std::string>;

    auto Render(const std::shared_ptr<Texture>& background) const -> void;

    ~GLBackgroundPass();

private:
    std::array<GLuint, 2> vao_ {};

    std::unique_ptr<GLProgram> background_2d_ {nullptr};
    std::unique_ptr<GLProgram> background_cube_ {nullptr};

    GLuint vbo_ {0};

    auto InitializeBackground2D() -> std::expected<void, std::string>;

    auto InitializeBackgroundCube() -> std::expected<void, std::string>;
};

}