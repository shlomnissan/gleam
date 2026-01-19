/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "renderer/gl/gl_textures.hpp"

#include "vglx/textures/dynamic_texture_2d.hpp"
#include "vglx/textures/texture_2d.hpp"

#include "utilities/logger.hpp"
#include "utilities/assert.hpp"

#include <algorithm>
#include <utility>

namespace vglx {

namespace {

struct DynamicTextureFormat {
    int internal_format;
    int format;
    int type;
};

auto get_dynamic_texture_format(const DynamicTexture2D* tex) -> DynamicTextureFormat;

}

auto GLTextures::Bind(const std::shared_ptr<Texture>& texture, int tex_unit) -> void {
    VGLX_ASSERT(
        tex_unit >= 0 && tex_unit < kMaxTextureUnits,
        "GLTextures::Bind texture unit out of range"
    );

    glActiveTexture(GL_TEXTURE0 + tex_unit);

    auto raw = texture.get();
    auto tex_id = texture->renderer_id;
    if (tex_id == 0) {
        tex_id = GenerateTexture(raw);
        textures_.emplace_back(texture);
    }

    if (tex_id != current_texture_ids_[tex_unit]) {
        glBindTexture(GL_TEXTURE_2D, tex_id);
        current_texture_ids_[tex_unit] = tex_id;
    }

    if (raw->GetType() == Texture::Type::DynamicTexture2D) {
        FlushDynamicTexture(static_cast<DynamicTexture2D*>(raw));
    }
}

auto GLTextures::Reset() -> void {
    std::ranges::fill(current_texture_ids_, 0);
}

auto GLTextures::GenerateTexture(Texture* texture) const -> GLuint {
    auto& tex_id = texture->renderer_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);

    glPixelStorei(
        GL_UNPACK_ALIGNMENT,
        std::to_underlying(texture->row_alignment)
    );

    if (texture->GetType() == Texture::Type::Texture2D) {
        auto tex = static_cast<Texture2D*>(texture);
        auto internal_format = texture->color_space == Texture::ColorSpace::Linear
            ? GL_RGBA8
            : GL_SRGB8_ALPHA8;

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            internal_format,
            tex->width,
            tex->height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            tex->data.data()
        );

        if (tex->generate_mipamps) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    if (texture->GetType() == Texture::Type::DynamicTexture2D) {
        auto tex = static_cast<DynamicTexture2D*>(texture);
        auto f = get_dynamic_texture_format(tex);
        for (auto level = 0u; level < tex->mips; ++level) {
            const auto width = std::max(1, tex->width >> level);
            const auto height = std::max(1, tex->height >> level);
            glTexImage2D(
                GL_TEXTURE_2D,
                static_cast<GLint>(level),
                f.internal_format,
                width,
                height,
                0,
                f.format,
                f.type,
                nullptr
            );
        }
        auto max_mip = static_cast<GLint>(tex->mips - 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, max_mip);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    if (glGetError() != GL_NO_ERROR) {
        Logger::Log(LogLevel::Error, "OpenGL error failed to generate texture");
    }

    texture->OnDispose([this](Disposable* target) {
        glDeleteTextures(1, &(static_cast<Texture*>(target)->renderer_id));
        Logger::Log(LogLevel::Debug, "Texture buffer cleared {}", *static_cast<Texture*>(target));
    });

    return tex_id;
}

auto GLTextures::FlushDynamicTexture(DynamicTexture2D* texture) const -> void {
    if (texture->pending_.empty()) return;

    auto f = get_dynamic_texture_format(texture);
    for (auto& u : texture->pending_) {
        glTexSubImage2D(
            GL_TEXTURE_2D,
            static_cast<GLint>(u.mip_level),
            u.x,
            u.y,
            u.w,
            u.h,
            f.format,
            f.type,
            u.bytes.data()
        );
    }

    texture->pending_.clear();
}

GLTextures::~GLTextures() {
    for (const auto& texture : textures_) {
        if (auto t = texture.lock()) t->Dispose();
    }
}

namespace {

auto get_dynamic_texture_format(const DynamicTexture2D* tex) -> DynamicTextureFormat {
    switch (tex->format) {
        case DynamicTexture2D::Format::RGBA8:
            return DynamicTextureFormat {
                .internal_format = GL_RGBA8,
                .format = GL_RGBA,
                .type = GL_UNSIGNED_BYTE
            };
        case DynamicTexture2D::Format::RGBA16F:
            return DynamicTextureFormat {
                .internal_format = GL_RGBA16F,
                .format = GL_RGBA,
                .type = GL_HALF_FLOAT
            };
        case DynamicTexture2D::Format::R16F:
            return DynamicTextureFormat {
                .internal_format = GL_R16F,
                .format = GL_RED,
                .type = GL_HALF_FLOAT
            };
        case DynamicTexture2D::Format::R32F:
            return DynamicTextureFormat {
                .internal_format = GL_R32F,
                .format = GL_RED,
                .type = GL_FLOAT
            };
        case DynamicTexture2D::Format::R32UI:
            return DynamicTextureFormat {
                .internal_format = GL_R32UI,
                .format = GL_RED_INTEGER,
                .type = GL_UNSIGNED_INT
            };
        default: VGLX_UNREACHABLE();
    }
}

}

}