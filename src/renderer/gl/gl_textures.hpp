/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <array>
#include <memory>
#include <utility>
#include <vector>

#include <glad/glad.h>

#include "vglx/textures/texture.hpp"

namespace vglx {

enum class GLTextureMapType {
    AlbedoMap = 0,
    AlphaMap = 1,
    NormalMap = 2,
    SpecularMap = 3,
    TextureMap = 4,
    EmissiveMap = 5,
    Reserved
};

constexpr auto kReservedTextureUnits = std::to_underlying(GLTextureMapType::Reserved);
constexpr auto kMaxTextureUnits = 16;

struct TextureFormat {
    int internal_format;
    int source_format;
    int type;
};

auto to_gl_tex_format(Texture::Format f) -> TextureFormat;

auto to_gl_min_filter(Texture::MinFilter f) -> int;

auto to_gl_mag_filter(Texture::MagFilter f) -> int;

class DynamicTexture2D;

class GLTextures {
public:
    GLTextures() = default;

    // delete copy constructor and assignment operator
    GLTextures(const GLTextures&) = delete;
    GLTextures& operator=(const GLTextures&) = delete;

    // delete move constructor and assignment operator
    GLTextures(GLTextures&&) = delete;
    GLTextures& operator=(GLTextures&&) = delete;

    auto Bind(const std::shared_ptr<Texture>& texture, int tex_unit) -> void;

    auto Reset() -> void;

    ~GLTextures();

private:
    std::vector<std::weak_ptr<Texture>> textures_;

    std::array<GLuint, kMaxTextureUnits> current_texture_ids_ {};

    auto GenerateTexture(Texture* texture) const -> GLuint;

    auto FlushDynamicTexture(DynamicTexture2D* texture) const -> void;
};

}