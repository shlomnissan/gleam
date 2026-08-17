/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <glad/glad.h>

#include "renderer/gl/gl_textures.hpp"

#include <string>
#include <vector>

namespace vglx {

class RenderTarget;
class GLTextures;

class GLFramebuffers {
public:
    GLFramebuffers() = default;

    GLFramebuffers(const GLFramebuffers&) = delete;
    GLFramebuffers(GLFramebuffers&&) = delete;

    auto operator=(const GLFramebuffers&) -> GLFramebuffers& = delete;
    auto operator=(GLFramebuffers&&) -> GLFramebuffers& = delete;

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

    std::vector<std::pair<std::string, GLFramebuffer>> framebuffers_ {};

    std::shared_ptr<bool> alive_ { std::make_shared<bool>(true) };

    auto GetFramebuffer(const std::string& uuid) -> GLFramebuffer*;

    auto CreateFramebuffer(RenderTarget* target) -> GLFramebuffer;

    auto DisposeFramebuffer(GLFramebuffer& framebuffer) -> void;
};

}
