/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "renderer/gl/gl_framebuffers.hpp"

#include "vglx/core/render_target.hpp"

#include "renderer/gl/gl_textures.hpp"
#include "utilities/logger.hpp"

#include <algorithm>

namespace vglx {

namespace {

constexpr GLenum kDepthStencilFormat = GL_DEPTH24_STENCIL8;

auto unbind_buffers() -> void {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

}

auto GLFramebuffers::CreateFramebuffer(RenderTarget* target) -> GLFramebuffer {
    auto framebuffer = GLFramebuffer { .fbo = 0 };
    auto format = to_gl_tex_format(target->format);

    glGenFramebuffers(1, &framebuffer.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);

    glGenTextures(1, &framebuffer.color_attachment);
    glBindTexture(GL_TEXTURE_2D, framebuffer.color_attachment);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format.internal_format,
        target->width,
        target->height,
        0,
        format.source_format,
        format.type,
        nullptr
    );

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        framebuffer.color_attachment,
        0
    );

    if (target->has_depth) {
        glGenRenderbuffers(1, &framebuffer.depth_attachment);
        glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.depth_attachment);
        glRenderbufferStorage(
            GL_RENDERBUFFER,
            kDepthStencilFormat,
            target->width,
            target->height
        );

        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER,
            framebuffer.depth_attachment
        );
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Logger::Log(LogLevel::Error, "Failed to create framebuffer object {}", *target);

        if (framebuffer.fbo) glDeleteFramebuffers(1, &framebuffer.fbo);
        if (framebuffer.color_attachment) glDeleteTextures(1, &framebuffer.color_attachment);
        if (framebuffer.depth_attachment) glDeleteRenderbuffers(1, &framebuffer.depth_attachment);
        unbind_buffers();

        return GLFramebuffer { .fbo = 0 };
    }

    target->renderer_id = framebuffer.fbo;
    framebuffers_.emplace_back(target, framebuffer);

    target->OnDispose([this](Disposable* d) {
        auto* target = static_cast<RenderTarget*>(d);

        auto it = std::ranges::find_if(framebuffers_, [target](const auto& e) {
            return e.first == target;
        });

        if (it != framebuffers_.end()) {
            target->renderer_id = 0;
            DisposeFramebuffer(it->second);
            framebuffers_.erase(it);
            Logger::Log(LogLevel::Debug, "Framebuffer object cleared", *target);
        }
    });

    unbind_buffers();

    return framebuffer;
}

auto GLFramebuffers::GetFramebuffer(RenderTarget* target) -> GLFramebuffer {
    auto it = std::ranges::find_if(framebuffers_, [target](const auto& entry) {
        return entry.first == target;
    });
    if (it != framebuffers_.end()) return it->second;

    return CreateFramebuffer(target);
}

auto GLFramebuffers::Begin(RenderTarget* target) -> void {
    auto framebuffer = GetFramebuffer(target);
    if (framebuffer.fbo == 0) {
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
    glViewport(0, 0, target->width, target->height);
    current_fbo_ = framebuffer.fbo;
}

auto GLFramebuffers::End() -> void {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    current_fbo_ = 0;
}

auto GLFramebuffers::GetColorAttachment(RenderTarget* target) -> unsigned int {
    auto framebuffer = GetFramebuffer(target);
    return framebuffer.color_attachment;
}

auto GLFramebuffers::Reset() -> void {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    current_fbo_ = 0;
}

auto GLFramebuffers::DisposeFramebuffer(const GLFramebuffer& framebuffer) -> void {
    if (framebuffer.fbo) glDeleteFramebuffers(1, &framebuffer.fbo);
    if (framebuffer.color_attachment) glDeleteTextures(1, &framebuffer.color_attachment);
    if (framebuffer.depth_attachment) glDeleteRenderbuffers(1, &framebuffer.depth_attachment);
}

GLFramebuffers::~GLFramebuffers() {
    for (auto& [target, framebuffer] : framebuffers_) {
        target->renderer_id = 0;
        target->RemoveDisposeHandlers();
        DisposeFramebuffer(framebuffer);
        Logger::Log(LogLevel::Debug, "Framebuffer object cleared", *target);
    }
    framebuffers_.clear();
}

}