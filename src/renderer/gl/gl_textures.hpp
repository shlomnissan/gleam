/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <glad/glad.h>

#include "vglx/textures/texture.hpp"

namespace vglx {

struct TextureFormat {
    int internal_format;
    int source_format;
    int type;
};

auto to_gl_tex_type(const Texture* t) -> int;

auto to_gl_tex_format(Texture::Format f) -> TextureFormat;

auto to_gl_min_filter(Texture::MinFilter f) -> int;

auto to_gl_mag_filter(Texture::MagFilter f) -> int;

auto to_gl_wrap(Texture::Wrapping w) -> int;

class DynamicTexture2D;

class GLTextures {
public:
    GLTextures();

    GLTextures(const GLTextures&) = delete;
    GLTextures(GLTextures&&) = delete;

    auto operator=(const GLTextures&) -> GLTextures& = delete;
    auto operator=(GLTextures&&) -> GLTextures& = delete;

    [[nodiscard]] auto Bind(const std::shared_ptr<Texture>& texture, uint8_t texture_unit) -> GLuint;

    [[nodiscard]] auto GetTextureId(const std::shared_ptr<Texture>& texture) -> GLuint;

    auto Reset() -> void;

    ~GLTextures();

private:
    std::unordered_map<std::string, GLuint> cache_ {};

    std::vector<GLuint> current_texture_ids_ {};

    std::shared_ptr<bool> alive_ { std::make_shared<bool>(true) };

    auto GenerateTexture(Texture* texture) -> GLuint;

    auto FlushDynamicTexture(DynamicTexture2D* texture) const -> void;
};

}
