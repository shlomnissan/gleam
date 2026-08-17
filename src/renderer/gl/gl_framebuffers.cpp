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

    framebuffers_.emplace_back(target->UUID(), framebuffer);

    target->OnDispose([this, alive = std::weak_ptr(alive_), name = target->Name().empty() ? target->UUID() : target->Name()](const std::string& target_uuid) {
        if (alive.expired()) return;
        auto it = std::ranges::find(framebuffers_, target_uuid, &std::pair<std::string, GLFramebuffer>::first);
        if (it != framebuffers_.end()) {
            DisposeFramebuffer(it->second);
            framebuffers_.erase(it);
            Logger::Log(LogLevel::Debug, "Framebuffer object cleared {}", name);
        }
    });

    unbind_buffers();

    return framebuffer;
}

auto GLFramebuffers::GetFramebuffer(const std::string& uuid) -> GLFramebuffer* {
    auto it = std::ranges::find(framebuffers_, uuid, &std::pair<std::string, GLFramebuffer>::first);
    return it != framebuffers_.end() ? &it->second : nullptr;
}

auto GLFramebuffers::Begin(RenderTarget* target) -> void {
    if (target->Disposed()) {
        auto name = target->Name().empty() ? target->UUID() : target->Name();
        Logger::Log(LogLevel::Error, "Failed to bind render target {}. Render target marked as disposed", name);
        return;
    }

    auto* existing = GetFramebuffer(target->UUID());
    auto framebuffer = existing != nullptr ? *existing : CreateFramebuffer(target);
    if (framebuffer.fbo == 0) {
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
    glViewport(0, 0, target->width, target->height);
    current_fbo_ = framebuffer.fbo;
}

auto GLFramebuffers::End(RenderTarget* target) -> void {
    if (target->enable_readback && !target->Disposed()) {
        if (auto* framebuffer = GetFramebuffer(target->UUID())) {
            auto format = to_gl_tex_format(target->format);
            glBindTexture(GL_TEXTURE_2D, framebuffer->color_attachment);
            glGetTexImage(GL_TEXTURE_2D, 0, format.source_format, format.type, target->color_data_.data());
            target->has_readback_ = true;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    current_fbo_ = 0;
}

auto GLFramebuffers::GetColorAttachment(RenderTarget* target) -> unsigned int {
    if (auto* existing = GetFramebuffer(target->UUID())) {
        return existing->color_attachment;
    }
    return CreateFramebuffer(target).color_attachment;
}

auto GLFramebuffers::Reset() -> void {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    current_fbo_ = 0;
}

auto GLFramebuffers::DisposeFramebuffer(GLFramebuffer& framebuffer) -> void {
    if (framebuffer.fbo) glDeleteFramebuffers(1, &framebuffer.fbo);
    if (framebuffer.color_attachment) glDeleteTextures(1, &framebuffer.color_attachment);
    if (framebuffer.depth_attachment) glDeleteRenderbuffers(1, &framebuffer.depth_attachment);

    framebuffer.fbo = 0;
    framebuffer.color_attachment = 0;
    framebuffer.depth_attachment = 0;
}

GLFramebuffers::~GLFramebuffers() {
    for (auto& [_, framebuffer] : framebuffers_) {
        DisposeFramebuffer(framebuffer);
    }
}

}
