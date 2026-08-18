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
#include <string>
#include <utility>
#include <variant>

namespace vglx {

namespace {

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

constexpr auto gl_pixel_type = overload {
    [](const std::vector<std::uint8_t>&) -> GLenum { return GL_UNSIGNED_BYTE; },
    [](const std::vector<float>&) -> GLenum { return GL_FLOAT; }
};

auto pick_image_format(const Image& img, Texture::ColorSpace color_space) -> Texture::Format {
    return std::visit(overload {
        [](const std::vector<float>&) {
            return Texture::Format::RGBA16F;
        },
        [&](const std::vector<std::uint8_t>&) {
            return color_space == Texture::ColorSpace::Linear
                ? Texture::Format::RGBA8
                : Texture::Format::SRGBA8;
        }
    }, img.data);
}

}

// GL_EXT_texture_filter_anisotropic is core in GL 4.6
// but vglx currently targets 4.1 for macOS.
#ifndef GL_TEXTURE_MAX_ANISOTROPY
#define GL_TEXTURE_MAX_ANISOTROPY 0x84FE
#endif

namespace {

auto apply_sampler_params(GLenum target, const Texture* tex) -> void {
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, to_gl_min_filter(tex->min_filter));
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, to_gl_mag_filter(tex->mag_filter));

    glTexParameteri(target, GL_TEXTURE_WRAP_S, to_gl_wrap(tex->wrap_s));
    glTexParameteri(target, GL_TEXTURE_WRAP_T, to_gl_wrap(tex->wrap_t));

    if (tex->anisotropy > 1.0f) {
        const auto anisotropy = std::min(tex->anisotropy, gl::limits().max_anisotropy);
        glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
    }
}

}

auto GLTextures::Bind(const std::shared_ptr<Texture>& texture, uint8_t texture_unit) -> GLuint {
    VGLX_ASSERT(
        texture_unit < kMaxTextureUnits,
        "GLTextures::Bind texture unit out of range"
    );

    const auto texture_id = GetTextureId(texture);
    if (texture_id == 0) {
        return 0u;
    }

    glActiveTexture(GL_TEXTURE0 + texture_unit);

    if (texture_id != current_texture_ids_[texture_unit]) {
        glBindTexture(to_gl_tex_type(texture.get()), texture_id);
        current_texture_ids_[texture_unit] = texture_id;
    }

    if (texture->GetType() == Texture::Type::DynamicTexture2D) {
        FlushDynamicTexture(static_cast<DynamicTexture2D*>(texture.get()));
    }

    return texture_id;
}

auto GLTextures::GetTextureId(const std::shared_ptr<Texture>& texture) -> GLuint {
    if (texture->Disposed()) {
        Logger::Log(
            LogLevel::Error,
            "Failed to retrieve texture {}. Texture was already disposed",
            texture->DisplayName()
        );
        return 0u;
    }

    if (auto it = cache_.find(texture->UUID()); it != cache_.end()) {
        return it->second;
    }

    const auto texture_id = GenerateTexture(texture.get());
    cache_.emplace(texture->UUID(), texture_id);

    // GenerateTexture binds the new texture to the active unit,
    // invalidating the unit-binding cache.
    Reset();

    return texture_id;
}

auto GLTextures::Reset() -> void {
    std::ranges::fill(current_texture_ids_, 0);
}

auto GLTextures::GenerateTexture(Texture* texture) -> GLuint {
    auto texture_id = GLuint {0};

    glGenTextures(1, &texture_id);
    glBindTexture(to_gl_tex_type(texture), texture_id);

    glPixelStorei(
        GL_UNPACK_ALIGNMENT,
        std::to_underlying(texture->row_alignment)
    );

    if (texture->GetType() == Texture::Type::Texture2D) {
        auto tex = static_cast<Texture2D*>(texture);
        const auto has_pixels = !std::visit(
            [](const auto& pixels) { return pixels.empty(); },
            tex->image->data
        );

        if (has_pixels) {
            tex->format = pick_image_format(*tex->image, tex->color_space);
        }

        auto format = to_gl_tex_format(tex->format);
        if (has_pixels) {
            std::visit([&](const auto& pixels) {
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    format.internal_format,
                    tex->image->width,
                    tex->image->height,
                    0,
                    format.source_format,
                    gl_pixel_type(pixels),
                    pixels.data()
                );
            }, tex->image->data);

            if (tex->generate_mipamps) glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                format.internal_format,
                tex->image->width,
                tex->image->height,
                0,
                format.source_format,
                format.type,
                nullptr
            );
        }
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
        tex->format = pick_image_format(*tex->images.positive_x, tex->color_space);

        auto format = to_gl_tex_format(tex->format);

        auto upload_face = [&](GLenum target, const Image& img) {
            std::visit([&](const auto& pixels) {
                glTexImage2D(
                    target,
                    0,
                    format.internal_format,
                    img.width,
                    img.height,
                    0,
                    format.source_format,
                    gl_pixel_type(pixels),
                    pixels.data()
                );
            }, img.data);
        };

        upload_face(GL_TEXTURE_CUBE_MAP_POSITIVE_X, *tex->images.positive_x);
        upload_face(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, *tex->images.negative_x);
        upload_face(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, *tex->images.positive_y);
        upload_face(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, *tex->images.negative_y);
        upload_face(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, *tex->images.positive_z);
        upload_face(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, *tex->images.negative_z);

        if (tex->generate_mipamps) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        apply_sampler_params(GL_TEXTURE_CUBE_MAP, tex);

        // Cube maps always clamp to edge to avoid seams across faces.
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    if (glGetError() != GL_NO_ERROR) {
        Logger::Log(LogLevel::Error, "OpenGL error failed to generate texture");
    }

    texture->OnDispose([this, alive = std::weak_ptr(alive_), name = texture->DisplayName()](const std::string& texture_uuid) {
        if (alive.expired()) return;
        if (auto it = cache_.find(texture_uuid); it != cache_.end()) {
            glDeleteTextures(1, &it->second);
            std::ranges::replace(current_texture_ids_, it->second, 0u);
            cache_.erase(it);
            Logger::Log(LogLevel::Debug, "Texture buffer cleared {}", name);
        }
    });

    return texture_id;
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
    for (auto& [_, texture_id] : cache_) {
        glDeleteTextures(1, &texture_id);
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

auto to_gl_wrap(Texture::Wrapping w) -> int {
    using Wrap = Texture::Wrapping;
    switch (w) {
        case Wrap::Repeat: return GL_REPEAT;
        case Wrap::ClampToEdge: return GL_CLAMP_TO_EDGE;
        case Wrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
        default: VGLX_UNREACHABLE();
    }
}

}
