/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

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
    MetallicMap = 6,
    RoughnessMap = 7,
    AOMap = 8,
    EnvironmentMap = 9,
    IrradianceMap = 10,
    PrefilteredMap = 11,
    BrdfLutMap = 12,
    ShadowMap2D = 13,
    PointShadowMap = 14,
    Reserved
};

constexpr auto kReservedTextureUnits = std::to_underlying(GLTextureMapType::Reserved);
constexpr auto kMaxTextureUnits = 16;

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
    GLTextures() = default;

    GLTextures(const GLTextures&) = delete;
    GLTextures(GLTextures&&) = delete;

    auto operator=(const GLTextures&) -> GLTextures& = delete;
    auto operator=(GLTextures&&) -> GLTextures& = delete;

    auto Bind(const std::shared_ptr<Texture>& texture, uint8_t texture_unit) -> GLuint;

    auto GetTextureId(const std::shared_ptr<Texture>& texture) -> GLuint;

    auto Reset() -> void;

    ~GLTextures();

private:
    std::unordered_map<std::string, GLuint> cache_ {};

    std::array<GLuint, kMaxTextureUnits> current_texture_ids_ {};

    std::shared_ptr<bool> alive_ { std::make_shared<bool>(true) };

    auto GenerateTexture(Texture* texture) -> GLuint;

    auto FlushDynamicTexture(DynamicTexture2D* texture) const -> void;
};

}
