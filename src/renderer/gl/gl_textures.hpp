/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/textures/texture.hpp"

#include <array>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

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

    GLTextures(const GLTextures&) = delete;
    GLTextures(GLTextures&&) = delete;
    GLTextures& operator=(const GLTextures&) = delete;
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