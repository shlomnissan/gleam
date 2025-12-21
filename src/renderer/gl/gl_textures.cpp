/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "renderer/gl/gl_textures.hpp"

#include "vglx/textures/texture_2d.hpp"

#include "utilities/logger.hpp"

#include <utility>

namespace vglx {

auto GLTextures::Bind(
    const std::shared_ptr<Texture>& texture,
    GLTextureMapType map_type
) -> void {
    auto tex_unit = std::to_underlying(map_type);
    glActiveTexture(GL_TEXTURE0 + tex_unit);

    auto tex_id = texture->renderer_id;
    if (tex_id == 0) {
        tex_id = GenerateTexture(texture.get());
        textures_.emplace_back(texture);
    }

    if (tex_id == current_texture_ids_[tex_unit]) return;

    glBindTexture(GL_TEXTURE_2D, tex_id);
    current_texture_ids_[tex_unit] = tex_id;
}

auto GLTextures::GenerateTexture(Texture* texture) const -> GLuint {
    auto& tex_id = texture->renderer_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);

    // Currently, the engine only supports 2D textures.
    auto texture_2d = static_cast<Texture2D*>(texture);

    glPixelStorei(
        GL_UNPACK_ALIGNMENT,
        std::to_underlying(texture->row_alignment)
    );

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8, // Guaranteed by asset builder
        texture_2d->width,
        texture_2d->height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        texture_2d->data.data()
    );

    // Complete without mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (glGetError() != GL_NO_ERROR) {
        Logger::Log(LogLevel::Error, "OpenGL error failed to generate texture");
    }

    texture->OnDispose([this](Disposable* target) {
        glDeleteTextures(1, &(static_cast<Texture*>(target)->renderer_id));
        Logger::Log(LogLevel::Debug, "Texture buffer cleared {}", *static_cast<Texture*>(target));
    });

    return tex_id;
}

GLTextures::~GLTextures() {
    for (const auto& texture : textures_) {
        if (auto t = texture.lock()) t->Dispose();
    }
}

}