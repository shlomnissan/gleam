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
#include "utilities/logger.hpp"
#include "utilities/assert.hpp"

#include "shaders/internal/headers/equirect_to_cube_frag.h"
#include "shaders/internal/headers/equirect_to_cube_vert.h"
#include "shaders/internal/headers/irradiance_cube_frag.h"
#include "shaders/internal/headers/prefiltered_cube_frag.h"
#include "shaders/internal/headers/screen_triangle_vert.h"
#include "shaders/internal/headers/brdf_lut_frag.h"

#include <algorithm>
#include <array>

namespace vglx {

namespace {

constexpr auto kBaseCubeSize = 512;
constexpr auto kIrradianceSize = 32;
constexpr auto kPrefilterSize = 128;
constexpr auto kLutSize = 512;
constexpr auto kPrefilteredMips = 5;

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

auto create_cube_texture(int size, bool mips) {
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

    // Allocate the chain, garbage content gets overwritten
    if (mips) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return output;
}

auto create_2d_texture(int size, GLint internal_format) {
    auto output = GLuint {0};
    glGenTextures(1, &output);
    glBindTexture(GL_TEXTURE_2D, output);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internal_format,
        size,
        size,
        0,
        GL_RG,
        GL_HALF_FLOAT,
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

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

    prg_equirect_to_cube_ = std::make_unique<GLProgram>(std::vector<ShaderInfo> {
        {.type = ShaderType::kVertexShader, .source = _SHADER_equirect_to_cube_vert},
        {.type = ShaderType::kFragmentShader, .source = _SHADER_equirect_to_cube_frag}
    });

    if (!prg_equirect_to_cube_->IsValid()) {
        return std::unexpected("Unable to create equirect to cube program");
    }

    prg_irradiance_cube_ = std::make_unique<GLProgram>(std::vector<ShaderInfo> {
        {.type = ShaderType::kVertexShader, .source = _SHADER_equirect_to_cube_vert},
        {.type = ShaderType::kFragmentShader, .source = _SHADER_irradiance_cube_frag}
    });

    if (!prg_irradiance_cube_->IsValid()) {
        return std::unexpected("Unable to create irradiance cube program");
    }

    prg_prefiltered_cube_ = std::make_unique<GLProgram>(std::vector<ShaderInfo>{
        {.type = ShaderType::kVertexShader, .source = _SHADER_equirect_to_cube_vert},
        {.type = ShaderType::kFragmentShader, .source = _SHADER_prefiltered_cube_frag},
    });

    if (!prg_prefiltered_cube_->IsValid()) {
        return std::unexpected("Unable to create prefiltered cube program");
    }

    prg_brdf_lut_ = std::make_unique<GLProgram>(std::vector<ShaderInfo>{
        {.type = ShaderType::kVertexShader, .source = _SHADER_screen_triangle_vert},
        {.type = ShaderType::kFragmentShader, .source = _SHADER_brdf_lut_frag},
    });

    if (!prg_brdf_lut_->IsValid()) {
        return std::unexpected("Unable to create brdf lut program");
    }

    lut_ = create_2d_texture(kLutSize, GL_RG16F);
    if (lut_ == 0) {
        return std::unexpected("Failed to allocate storage for BRDF LUT");
    }

    GenerateBrdfLut();

    return {};
}

auto GLEnvironment::GenerateBrdfLut() -> void {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        lut_,
        0
    );

    VGLX_ASSERT(
        glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
        "BRDF LUT framebuffer is incomplete"
    );

    glUseProgram(prg_brdf_lut_->ProgramId());
    glViewport(0, 0, kLutSize, kLutSize);
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

auto GLEnvironment::GetOrProcess(const std::shared_ptr<Texture>& source) -> std::optional<GLEnvironmentMaps> {
    using enum Texture::Type;
    using enum Texture::Mapping;

    auto it = std::ranges::find(cache_, source->UUID(), &std::pair<std::string, GLEnvironmentMaps>::first);
    if (it != cache_.end()) {
        return it->second;
    }

    const auto type = source->GetType();
    const auto is_equirect_tex = type == Texture2D && source->mapping == Equirectangular;
    const auto is_cube_tex = type == CubeTexture;

    if (!is_equirect_tex && !is_cube_tex) {
        Logger::Log(LogLevel::Error, "Environment source must be equirectangular or a cube texture");
        return std::nullopt;
    }

    if (source->renderer_id == 0) {
        Logger::Log(
            LogLevel::Error,
            "Attempting to bind an unallocated environment texture"
        );
        return std::nullopt;
    }

    auto output = GLEnvironmentMaps {
        .prefiltered_mips = kPrefilteredMips,
    };

    output.irradiance = create_cube_texture(kIrradianceSize, false);
    output.prefiltered = create_cube_texture(kPrefilterSize, true);

    if (output.irradiance == 0 || output.prefiltered == 0) {
        dispose(output);
        Logger::Log(
            LogLevel::Error,
            "Failed to allocate storage for environment map processing"
        );
        return std::nullopt;
    }

    auto convolve_src = source->renderer_id;

    if (is_equirect_tex) {
        output.base_cube = create_cube_texture(kBaseCubeSize, true);
        if (output.base_cube == 0) {
            dispose(output);
            Logger::Log(
                LogLevel::Error,
                "Failed to allocate storage for environment map processing"
            );
            return std::nullopt;
        }
        EquirectToCubeMap(source->renderer_id, output.base_cube);
        convolve_src = output.base_cube;
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, convolve_src);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    if (is_cube_tex) {
        // Source cubes upload with a non-mipmap min filter but the convolution
        // shaders sample explicit levels via textureLod which only reaches above
        // the base level with a mipmap filter.
        glTexParameteri(
            GL_TEXTURE_CUBE_MAP,
            GL_TEXTURE_MIN_FILTER,
            GL_LINEAR_MIPMAP_LINEAR
        );
    }

    IrradianceMap(convolve_src, output.irradiance);
    PrefilteredMap(convolve_src, output.prefiltered);

    // The chain allocates more levels than we convolve so we cap sampling
    glBindTexture(GL_TEXTURE_CUBE_MAP, output.prefiltered);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, kPrefilteredMips - 1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    cache_.emplace_back(source->UUID(), output);

    source->OnDispose([this, alive = std::weak_ptr(alive_)](const std::string& texture_uuid) {
        if (alive.expired()) return;

        auto it = std::ranges::find(
            cache_,
            texture_uuid,
            &std::pair<std::string, GLEnvironmentMaps>::first
        );

        if (it != cache_.end()) {
            dispose(it->second);
            cache_.erase(it);
        }
    });

    return output;
}

auto GLEnvironment::RenderToCubeFaces(GLProgram* program, GLuint dst, int size, int mip) -> void {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glUseProgram(program->ProgramId());
    glViewport(0, 0, size, size);

    for (auto i = 0; i < 6; ++i) {
        bind_cube_face(dst, i, mip);

        if (i == 0) {
            VGLX_ASSERT(
                glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
                "Environment framebuffer is incomplete"
            );
        }

        program->SetUniform("u_FaceBasis", &kFaceBases[i]);
        program->UpdateUniforms();
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

auto GLEnvironment::EquirectToCubeMap(GLuint src, GLuint dst) -> void {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, src);
    glBindVertexArray(vao_);

    const auto unit = 0;
    prg_equirect_to_cube_->SetUniform("u_EquirectTexture", &unit);

    RenderToCubeFaces(prg_equirect_to_cube_.get(), dst, kBaseCubeSize);
}

auto GLEnvironment::IrradianceMap(GLuint src, GLuint dst) -> void {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, src);
    glBindVertexArray(vao_);

    const auto unit = 0;
    prg_irradiance_cube_->SetUniform(Uniform::EnvironmentMap, &unit);

    RenderToCubeFaces(prg_irradiance_cube_.get(), dst, kIrradianceSize);
}

auto GLEnvironment::PrefilteredMap(GLuint src, GLuint dst) -> void {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, src);
    glBindVertexArray(vao_);

    const auto unit = 0;
    prg_prefiltered_cube_->SetUniform(Uniform::EnvironmentMap, &unit);

    for (auto mip = 0; mip < kPrefilteredMips; ++mip) {
        auto size = kPrefilterSize >> mip;
        auto roughness = mip / static_cast<float>(kPrefilteredMips - 1);
        prg_prefiltered_cube_->SetUniform("u_Roughness", &roughness);
        RenderToCubeFaces(prg_prefiltered_cube_.get(), dst, size, mip);
    }
}

GLEnvironment::~GLEnvironment() {
    alive_.reset();

    for (auto& [_, map] : cache_) dispose(map);

    if (fbo_) glDeleteFramebuffers(1, &fbo_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
    if (lut_) glDeleteTextures(1, &lut_);
}

}
