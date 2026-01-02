/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "renderer/gl/gl_scene_buffer.hpp"

#include "utilities/logger.hpp"

#include <glad/glad.h>

#include <algorithm>

namespace vglx {

namespace {

constexpr GLenum kColorFormat = GL_RGBA8;
constexpr GLenum kDepthStencilFormat = GL_DEPTH24_STENCIL8;

}

struct GLSceneBuffer::Impl {
    GLuint resolve_fbo {0};
    GLuint resolve_color_tex {0};
    GLuint resolve_depth_stencil {0};

    int width {0};
    int height {0};

    bool is_msaa {false};

    explicit Impl(const GLSceneBuffer::Parameters& params)
        : width(params.framebuffer_width),
          height(params.framebuffer_height)
    {
        auto sample_count = std::max(params.sample_count, 1);
        is_msaa = sample_count > 1;
    }

    auto Init() -> std::expected<void, std::string> {
        if (width <= 0 || height <= 0) {
            return std::unexpected("Scene buffer invalid size");
        }

        DeleteBuffers();
        return is_msaa ? InitWithMSAA() : InitWithoutMSAA();
    }

    auto InitWithoutMSAA() -> std::expected<void, std::string> {
        glGenFramebuffers(1, &resolve_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, resolve_fbo);

        glGenTextures(1, &resolve_color_tex);
        glBindTexture(GL_TEXTURE_2D, resolve_color_tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            kColorFormat,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            nullptr
        );

        glGenRenderbuffers(1, &resolve_depth_stencil);
        glBindRenderbuffer(GL_RENDERBUFFER, resolve_depth_stencil);
        glRenderbufferStorage(
            GL_RENDERBUFFER,
            kDepthStencilFormat,
            width,
            height
        );

        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            resolve_color_tex,
            0
        );

        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER,
            resolve_depth_stencil
        );

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            UnbindBuffers();
            Logger::Log(LogLevel::Error, "Failed to create a scene buffer");
            return std::unexpected("Failed to create a scene buffer");
        }

        UnbindBuffers();
        return {};
    }

    auto InitWithMSAA() -> std::expected<void, std::string> {
        return std::unexpected("Scene buffer MSAA not implemented yet");
    }

    auto UnbindBuffers() -> void {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    auto DeleteBuffers() -> void {
        if (!is_msaa) {
            if (resolve_fbo) glDeleteFramebuffers(1, &resolve_fbo);
            if (resolve_color_tex) glDeleteTextures(1, &resolve_color_tex);
            if (resolve_depth_stencil) glDeleteRenderbuffers(1, &resolve_depth_stencil);

            resolve_fbo = 0;
            resolve_color_tex = 0;
            resolve_depth_stencil = 0;
        }
    }
};

GLSceneBuffer::GLSceneBuffer(const Parameters& params)
  : impl_(std::make_unique<GLSceneBuffer::Impl>(params)) {}

auto GLSceneBuffer::Init() -> std::expected<void, std::string> {
    return impl_->Init();
}

auto GLSceneBuffer::Begin() -> void {}

auto GLSceneBuffer::End() -> void {}

GLSceneBuffer::~GLSceneBuffer() {
    impl_->DeleteBuffers();
}

}