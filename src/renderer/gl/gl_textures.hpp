/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/textures/texture.hpp"

#include <array>
#include <utility>

#include <glad/glad.h>

namespace vglx {

enum class GLTextureMapType {
    AlbedoMap = 0,
    AlphaMap = 1,
    NormalMap = 2,
    SpecularMap = 3,
    TextureMap = 4,
    Reserved
};

constexpr auto kReservedTextureUnits = std::to_underlying(GLTextureMapType::Reserved);
constexpr auto kMaxTextureUnits = 16;

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

    auto Bind(Texture* texture, int tex_unit) -> void;

    auto Reset() -> void;

private:
    std::array<GLuint, kMaxTextureUnits> current_texture_ids_ {};

    auto GenerateTexture(Texture* texture) const -> GLuint;

    auto FlushDynamicTexture(DynamicTexture2D* texture) const -> void;
};

}