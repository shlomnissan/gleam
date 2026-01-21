/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <glad/glad.h>

namespace vglx {

class RenderTarget;

class GLFramebuffers {
public:
    GLFramebuffers() = default;

    // delete copy constructor and assignment operator
    GLFramebuffers(const GLFramebuffers&) = delete;
    GLFramebuffers& operator=(const GLFramebuffers&) = delete;

    // delete move constructor and assignment operator
    GLFramebuffers(GLFramebuffers&&) = delete;
    GLFramebuffers& operator=(GLFramebuffers&&) = delete;

    auto Begin(RenderTarget* target) -> void;

    auto End(RenderTarget* target) -> void;

    auto Reset() -> void;

private:
    GLuint current_fbo_ {0};
};

}