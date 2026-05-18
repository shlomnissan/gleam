/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "renderer/gl/gl_textures.hpp"

#include "vglx/textures/cube_texture.hpp"
#include "vglx/textures/dynamic_texture_2d.hpp"
#include "vglx/textures/texture.hpp"
#include "vglx/textures/texture_2d.hpp"

#include "renderer/gl/gl_device.hpp"
#include "utilities/logger.hpp"
#include "utilities/assert.hpp"

#include <algorithm>
#include <memory>
#include <utility>

namespace vglx {

// GL_EXT_texture_filter_anisotropic; core in GL 4.6 but GLAD targets 4.1 on macOS.
#ifndef GL_TEXTURE_MAX_ANISOTROPY
#define GL_TEXTURE_MAX_ANISOTROPY 0x84FE
#endif

namespace {

auto apply_sampler_params(GLenum target, const Texture* tex) -> void {
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, to_gl_min_filter(tex->min_filter));
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, to_gl_mag_filter(tex->mag_filter));

    if (tex->anisotropy > 1.0f) {
        const auto anisotropy = std::min(tex->anisotropy, gl::limits().max_anisotropy);
        glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
    }
}

}

auto GLTextures::Bind(const std::shared_ptr<Texture>& texture, int tex_unit) -> void {
    VGLX_ASSERT(
        tex_unit >= 0 && tex_unit < kMaxTextureUnits,
        "GLTextures::Bind texture unit out of range"
    );

    glActiveTexture(GL_TEXTURE0 + tex_unit);

    auto tex_id = texture->renderer_id;
    if (tex_id == 0) {
        tex_id = GenerateTexture(texture.get());
        textures_.emplace_back(texture);
    }

    if (tex_id != current_texture_ids_[tex_unit]) {
        glBindTexture(to_gl_tex_type(texture.get()), tex_id);
        current_texture_ids_[tex_unit] = tex_id;
    }

    if (texture->GetType() == Texture::Type::DynamicTexture2D) {
        FlushDynamicTexture(static_cast<DynamicTexture2D*>(texture.get()));
    }
}

auto GLTextures::Reset() -> void {
    std::ranges::fill(current_texture_ids_, 0);
}

auto GLTextures::GenerateTexture(Texture* texture) const -> GLuint {
    auto& tex_id = texture->renderer_id;
    glGenTextures(1, &tex_id);
    glBindTexture(to_gl_tex_type(texture), tex_id);

    glPixelStorei(
        GL_UNPACK_ALIGNMENT,
        std::to_underlying(texture->row_alignment)
    );

    if (texture->GetType() == Texture::Type::Texture2D) {
        auto tex = static_cast<Texture2D*>(texture);
        tex->format = tex->color_space == Texture::ColorSpace::Linear
            ? Texture::Format::RGBA8
            : Texture::Format::SRGBA8;

        auto format = to_gl_tex_format(tex->format);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            format.internal_format,
            tex->image->width,
            tex->image->height,
            0,
            format.source_format,
            format.type,
            tex->image->data.data()
        );

        if (tex->generate_mipamps) glGenerateMipmap(GL_TEXTURE_2D);
        apply_sampler_params(GL_TEXTURE_2D, tex);
    }

    if (texture->GetType() == Texture::Type::DynamicTexture2D) {
        auto tex = static_cast<DynamicTexture2D*>(texture);
        auto format = to_gl_tex_format(tex->format);
        for (auto level = 0u; level < tex->mips; ++level) {
            const auto width = std::max(1, tex->width >> level);
            const auto height = std::max(1, tex->height >> level);
            glTexImage2D(
                GL_TEXTURE_2D,
                static_cast<GLint>(level),
                format.internal_format,
                width,
                height,
                0,
                format.source_format,
                format.type,
                nullptr
            );
        }

        auto max_mip = static_cast<GLint>(tex->mips - 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, max_mip);
        apply_sampler_params(GL_TEXTURE_2D, tex);
    }

    if (texture->GetType() == Texture::Type::CubeTexture) {
        auto tex = static_cast<CubeTexture*>(texture);
        tex->format = tex->color_space == Texture::ColorSpace::Linear
            ? Texture::Format::RGBA8
            : Texture::Format::SRGBA8;

        auto format = to_gl_tex_format(tex->format);

        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X,
            0,
            format.internal_format,
            tex->images.positive_x->width,
            tex->images.positive_x->height,
            0,
            format.source_format,
            format.type,
            tex->images.positive_x->data.data()
        );

        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
            0,
            format.internal_format,
            tex->images.negative_x->width,
            tex->images.negative_x->height,
            0,
            format.source_format,
            format.type,
            tex->images.negative_x->data.data()
        );

        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
            0,
            format.internal_format,
            tex->images.positive_y->width,
            tex->images.positive_y->height,
            0,
            format.source_format,
            format.type,
            tex->images.positive_y->data.data()
        );

        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
            0,
            format.internal_format,
            tex->images.negative_y->width,
            tex->images.negative_y->height,
            0,
            format.source_format,
            format.type,
            tex->images.negative_y->data.data()
        );

        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
            0,
            format.internal_format,
            tex->images.positive_z->width,
            tex->images.positive_z->height,
            0,
            format.source_format,
            format.type,
            tex->images.positive_z->data.data()
        );

        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
            0,
            format.internal_format,
            tex->images.negative_z->width,
            tex->images.negative_z->height,
            0,
            format.source_format,
            format.type,
            tex->images.negative_z->data.data()
        );

        if (tex->generate_mipamps) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        apply_sampler_params(GL_TEXTURE_CUBE_MAP, tex);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    if (glGetError() != GL_NO_ERROR) {
        Logger::Log(LogLevel::Error, "OpenGL error failed to generate texture");
    }

    texture->OnDispose([](Disposable* target) {
        glDeleteTextures(1, &(static_cast<Texture*>(target)->renderer_id));
        Logger::Log(LogLevel::Debug, "Texture buffer cleared {}", *static_cast<Texture*>(target));
    });

    return tex_id;
}

auto GLTextures::FlushDynamicTexture(DynamicTexture2D* texture) const -> void {
    if (texture->pending_.empty()) return;

    auto format = to_gl_tex_format(texture->format);
    for (auto& u : texture->pending_) {
        glTexSubImage2D(
            GL_TEXTURE_2D,
            static_cast<GLint>(u.mip_level),
            u.x,
            u.y,
            u.w,
            u.h,
            format.source_format,
            format.type,
            u.bytes.data()
        );
    }

    texture->pending_.clear();
}

GLTextures::~GLTextures() {
    // Ensure GPU resources owned by texture objects are released
    // while the OpenGL context is still valid.
    //
    // Texture instances may outlive the renderer (e.g. due to static
    // references in examples). By explicitly disposing them here, we
    // avoid calling glDelete* from late static destructors after the
    // OpenGL context has already been destroyed.
    for (const auto& texture : textures_) {
        if (auto t = texture.lock()) t->Dispose();
    }
}

auto to_gl_tex_format(Texture::Format f) -> TextureFormat {
    switch (f) {
        case Texture::Format::RGBA8:
            return TextureFormat {
                .internal_format = GL_RGBA8,
                .source_format = GL_RGBA,
                .type = GL_UNSIGNED_BYTE
            };
        case Texture::Format::RGBA16F:
            return TextureFormat {
                .internal_format = GL_RGBA16F,
                .source_format = GL_RGBA,
                .type = GL_HALF_FLOAT
            };
        case Texture::Format::R16F:
            return TextureFormat {
                .internal_format = GL_R16F,
                .source_format = GL_RED,
                .type = GL_HALF_FLOAT
            };
        case Texture::Format::R32F:
            return TextureFormat {
                .internal_format = GL_R32F,
                .source_format = GL_RED,
                .type = GL_FLOAT
            };
        case Texture::Format::R32UI:
            return TextureFormat {
                .internal_format = GL_R32UI,
                .source_format = GL_RED_INTEGER,
                .type = GL_UNSIGNED_INT
            };
        case Texture::Format::SRGBA8:
            return TextureFormat {
                .internal_format = GL_SRGB8_ALPHA8,
                .source_format = GL_RGBA,
                .type = GL_UNSIGNED_BYTE
            };
        default: VGLX_UNREACHABLE();
    }
}

auto to_gl_tex_type(const Texture* tex) -> int {
    using enum Texture::Type;
    switch(tex->GetType()) {
        case DynamicTexture2D:
        case Texture2D:
            return GL_TEXTURE_2D;
        case CubeTexture:
            return GL_TEXTURE_CUBE_MAP;
        default:
            VGLX_UNREACHABLE();
    }
}

auto to_gl_min_filter(Texture::MinFilter f) -> int {
    using Filter = Texture::MinFilter;
    switch (f) {
        case Filter::Nearest: return GL_NEAREST;
        case Filter::Linear: return GL_LINEAR;
        case Filter::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
        case Filter::LinearMipmapNearest: return GL_LINEAR_MIPMAP_NEAREST;
        case Filter::NearestMipmapLinear: return GL_NEAREST_MIPMAP_LINEAR;
        case Filter::LinearMipmapLinear: return GL_LINEAR_MIPMAP_LINEAR;
        default: VGLX_UNREACHABLE();
    }
}

auto to_gl_mag_filter(Texture::MagFilter f) -> int {
    using Filter = Texture::MagFilter;
    switch (f) {
        case Filter::Nearest: return GL_NEAREST;
        case Filter::Linear: return GL_LINEAR;
        default: VGLX_UNREACHABLE();
    }
}

}