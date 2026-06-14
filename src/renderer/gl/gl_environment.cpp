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
    return 0;
}

auto bind_cube_face(GLuint cube, int face, int mip, int size) {
    return;
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

    if (source->renderer_id == 0) {
        Logger::Log(
            LogLevel::Error,
            "Attempting to bind an unallocated environment texture"
        );
        return std::nullopt;
    }

    auto output = GLEnvironmentMaps {.base_size = kBaseCubeSize};

    // TODO: implement

    return output;
}

auto GLEnvironment::EquirectToCube(GLuint src_equirect, GLuint dst_cube, int size) -> void {
    // TODO: implement

    return;
}

GLEnvironment::~GLEnvironment() {
    for (auto& [_, map] : cache_) dispose(map);

    if (fbo_) glDeleteFramebuffers(1, &fbo_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
}

}