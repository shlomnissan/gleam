/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <glad/glad.h>

#include <memory>
#include <string>
#include <vector>

namespace vglx {

class RenderTarget;
class GLTextures;

class GLFramebuffers {
public:
    explicit GLFramebuffers(GLTextures& textures) : textures_(textures) {}

    GLFramebuffers(const GLFramebuffers&) = delete;
    GLFramebuffers(GLFramebuffers&&) = delete;

    auto operator=(const GLFramebuffers&) -> GLFramebuffers& = delete;
    auto operator=(GLFramebuffers&&) -> GLFramebuffers& = delete;

    auto Begin(RenderTarget* target) -> void;

    auto End(RenderTarget* target) -> void;

    auto Reset() -> void;

    ~GLFramebuffers();

private:
    GLTextures& textures_;

    GLuint current_fbo_ {0};

    struct GLFramebuffer {
        GLuint fbo;
        GLuint color_attachment;
        GLuint depth_attachment;
    };

    std::vector<std::pair<std::string, GLFramebuffer>> cache_ {};

    std::shared_ptr<bool> alive_ { std::make_shared<bool>(true) };

    auto GetFramebuffer(const std::string& uuid) -> GLFramebuffer*;

    auto CreateFramebuffer(RenderTarget* target) -> GLFramebuffer;

    auto DisposeFramebuffer(GLFramebuffer& framebuffer) -> void;
};

}
