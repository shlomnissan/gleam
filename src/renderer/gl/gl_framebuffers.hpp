/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <glad/glad.h>

#include "renderer/gl/gl_textures.hpp"

#include <vector>

namespace vglx {

class RenderTarget;
class GLTextures;

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

    auto GetColorAttachment(RenderTarget* target) -> unsigned int;

    auto Reset() -> void;

    ~GLFramebuffers();

private:
    GLuint current_fbo_ {0};

    struct GLFramebuffer {
        GLuint fbo;
        GLuint color_attachment;
        GLuint depth_attachment;
    };

    std::vector<std::pair<RenderTarget*, GLFramebuffer>> framebuffers_ {};

    auto GetFramebuffer(RenderTarget* target) -> GLFramebuffer;

    auto CreateFramebuffer(RenderTarget* target) -> GLFramebuffer;

    auto DisposeFramebuffer(const GLFramebuffer& framebuffer) -> void;
};

}