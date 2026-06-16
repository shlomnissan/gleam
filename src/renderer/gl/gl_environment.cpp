/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "renderer/gl/gl_environment.hpp"

#include "vglx/math/matrix3.hpp"
#include "vglx/textures/texture.hpp"

#include "core/shader_library.hpp"
#include "renderer/gl/gl_program.hpp"
#include "shaders/internal/headers/equirect_to_cube_frag.h"
#include "shaders/internal/headers/equirect_to_cube_vert.h"
#include "utilities/logger.hpp"
#include "utilities/assert.hpp"

#include <algorithm>
#include <array>

namespace vglx {

namespace {

constexpr auto kBaseCubeSize = 512;

constexpr auto kFaceBases = std::array<Matrix3, 6> {
    Matrix3(Vector3 { 0.0f,  0.0f, -1.0f}, Vector3 { 0.0f, -1.0f,  0.0f}, Vector3 { 1.0f,  0.0f,  0.0f}), // +X
    Matrix3(Vector3 { 0.0f,  0.0f,  1.0f}, Vector3 { 0.0f, -1.0f,  0.0f}, Vector3 {-1.0f,  0.0f,  0.0f}), // -X
    Matrix3(Vector3 { 1.0f,  0.0f,  0.0f}, Vector3 { 0.0f,  0.0f,  1.0f}, Vector3 { 0.0f,  1.0f,  0.0f}), // +Y
    Matrix3(Vector3 { 1.0f,  0.0f,  0.0f}, Vector3 { 0.0f,  0.0f, -1.0f}, Vector3 { 0.0f, -1.0f,  0.0f}), // -Y
    Matrix3(Vector3 { 1.0f,  0.0f,  0.0f}, Vector3 { 0.0f, -1.0f,  0.0f}, Vector3 { 0.0f,  0.0f,  1.0f}), // +Z
    Matrix3(Vector3 {-1.0f,  0.0f,  0.0f}, Vector3 { 0.0f, -1.0f,  0.0f}, Vector3 { 0.0f,  0.0f, -1.0f}), // -Z
};

auto dispose(GLEnvironmentMaps& maps) {
    if (maps.base_cube) glDeleteTextures(1, &maps.base_cube);
    if (maps.irradiance) glDeleteTextures(1, &maps.irradiance);
    if (maps.prefiltered) glDeleteTextures(1, &maps.prefiltered);

    maps.base_cube = 0;
    maps.irradiance = 0;
    maps.prefiltered = 0;
}

auto create_cube_texture(int size, bool mips) -> GLuint {
    auto output = GLuint {0};
    glGenTextures(1, &output);
    glBindTexture(GL_TEXTURE_CUBE_MAP, output);

    for (auto i = 0; i < 6; ++i) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            GL_RGBA16F,
            size,
            size,
            0,
            GL_RGBA,
            GL_HALF_FLOAT,
            nullptr
        );
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, mips ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return output;
}

auto bind_cube_face(GLuint texture, int face, int mip) {
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
        texture,
        mip
    );
}

}

auto GLEnvironment::Initialize() -> std::expected<void, std::string> {
    glGenFramebuffers(1, &fbo_);
    if (fbo_ == 0) {
        return std::unexpected("Environment manager failed to generate frame buffer object");
    }

    glGenVertexArrays(1, &vao_);
    if (vao_ == 0) {
        return std::unexpected("Environment manager failed to generate vertex array object");
    }

    equirect_to_cube_ = std::make_unique<GLProgram>(std::vector<ShaderInfo> {
        {.type = ShaderType::kVertexShader, .source = _SHADER_equirect_to_cube_vert},
        {.type = ShaderType::kFragmentShader, .source = _SHADER_equirect_to_cube_frag}
    });

    if (!equirect_to_cube_->IsValid()) {
        return std::unexpected("Unable to create equirect to cube program");
    }

    return {};
}

auto GLEnvironment::GetOrProcess(const std::shared_ptr<Texture>& source) -> std::optional<GLEnvironmentMaps> {
    auto it = std::ranges::find(cache_, source.get(), &std::pair<Texture*, GLEnvironmentMaps>::first);
    if (it != cache_.end()) {
        return it->second;
    }

    if (source->GetType() != Texture::Type::Texture2D ||
        source->mapping != Texture::Mapping::Equirectangular)
    {
        Logger::Log(LogLevel::Error, "Environment source must be an equirectangular 2D texture");
        return std::nullopt;
    }

    if (source->renderer_id == 0) {
        Logger::Log(
            LogLevel::Error,
            "Attempting to bind an unallocated environment texture"
        );
        return std::nullopt;
    }

    auto output = GLEnvironmentMaps {.base_size = kBaseCubeSize};

    output.base_cube = create_cube_texture(kBaseCubeSize, true);
    if (output.base_cube == 0) {
        Logger::Log(
            LogLevel::Error,
            "Failed to allocate storage for environment map processing"
        );
        return std::nullopt;
    }

    EquirectToCube(source->renderer_id, output.base_cube, output.base_size);

    glBindTexture(GL_TEXTURE_CUBE_MAP, output.base_cube);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    cache_.emplace_back(source.get(), output);

    source->OnDispose([this](Disposable* target) {
        auto it = std::ranges::find(
            cache_, static_cast<Texture*>(target),
            &std::pair<Texture*, GLEnvironmentMaps>::first
        );

        if (it != cache_.end()) {
            dispose(it->second);
            cache_.erase(it);
        }
    });

    return output;
}

auto GLEnvironment::EquirectToCube(GLuint src, GLuint dst, int size) -> void {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glUseProgram(equirect_to_cube_->Id());
    glViewport(0, 0, size, size);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, src);
    glBindVertexArray(vao_);

    const auto unit = 0;
    equirect_to_cube_->SetUniform("u_EquirectTexture", &unit);

    for (auto i = 0; i < 6; ++i) {
        bind_cube_face(dst, i, 0);

        if (i == 0) {
            VGLX_ASSERT(
                glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
                "Environment framebuffer is incomplete"
            );
        }

        equirect_to_cube_->SetUniform("u_FaceBasis", &kFaceBases[i]);
        equirect_to_cube_->UpdateUniforms();
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLEnvironment::~GLEnvironment() {
    for (auto& [_, map] : cache_) dispose(map);

    if (fbo_) glDeleteFramebuffers(1, &fbo_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
}

}