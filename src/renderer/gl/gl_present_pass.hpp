/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "renderer/gl/gl_program.hpp"

#include <expected>
#include <memory>
#include <string>

#include <glad/glad.h>

namespace vglx {

class GLSceneBuffer;

class GLPresentPass {
public:
    GLPresentPass() = default;

    // Non-copyable
    GLPresentPass(const GLPresentPass&) = delete;
    auto operator=(const GLPresentPass&) -> GLPresentPass& = delete;

    // Non-moveable
    GLPresentPass(GLPresentPass&&) = delete;
    auto operator=(GLPresentPass&&) -> GLPresentPass& = delete;

    auto Initialize() -> std::expected<void, std::string>;

    auto Present(const GLSceneBuffer& scene_buffer) const -> void;

    ~GLPresentPass();

private:
    GLuint vao_ {0};

    std::unique_ptr<GLProgram> program_ {nullptr};
};

}
