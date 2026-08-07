/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "renderer/gl/gl_background_pass.hpp"

#include "core/shader_library.hpp"

#include "renderer/gl/gl_uniform.hpp"
#include "shaders/internal/headers/background_cube_frag.h"
#include "shaders/internal/headers/background_cube_vert.h"
#include "shaders/internal/headers/background_equirect_frag.h"
#include "shaders/internal/headers/background_frag.h"
#include "shaders/internal/headers/screen_triangle_vert.h"
#include "utilities/assert.hpp"
#include "utilities/logger.hpp"

#include <vector>

namespace vglx {

namespace {

constexpr auto unit_cube = std::array<float, 108> {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

}

auto GLBackgroundPass::GenerateVAOs() -> std::expected<void, std::string> {
    glGenVertexArrays(2, vao_.data());
    if (vao_[0] == 0 || vao_[1] == 0) {
        return std::unexpected("Failed to generate background pass vertex array object");
    }

    glGenBuffers(1, &vbo_);

    if (vbo_ == 0) {
        return std::unexpected("Failed to generate vertex buffer for background cube");
    }

    glBindVertexArray(vao_[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(
        GL_ARRAY_BUFFER,
        unit_cube.size() * sizeof(GLfloat),
        unit_cube.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    return {};
}

auto GLBackgroundPass::InitializeBackground2D() -> std::expected<void, std::string> {
    background_2d_ = std::make_unique<GLProgram>(std::vector<ShaderInfo> {
        {.type = ShaderType::kVertexShader, .source = _SHADER_screen_triangle_vert},
        {.type = ShaderType::kFragmentShader, .source = _SHADER_background_frag}
    });

    if (!background_2d_->IsValid()) {
        return std::unexpected("Unable to create 2d background pass program");
    }

    return {};
}

auto GLBackgroundPass::InitializeBackgroundCube() -> std::expected<void, std::string> {
    background_cube_ = std::make_unique<GLProgram>(std::vector<ShaderInfo> {
        {.type = ShaderType::kVertexShader, .source = _SHADER_background_cube_vert},
        {.type = ShaderType::kFragmentShader, .source = _SHADER_background_cube_frag}
    });

    if (!background_cube_->IsValid()) {
        return std::unexpected("Unable to create cube background pass program");
    }

    return {};
}

auto GLBackgroundPass::InitializeBackgroundEquirect() -> std::expected<void, std::string> {
    background_equirect_ = std::make_unique<GLProgram>(std::vector<ShaderInfo> {
        {.type = ShaderType::kVertexShader, .source = _SHADER_background_cube_vert},
        {.type = ShaderType::kFragmentShader, .source = _SHADER_background_equirect_frag}
    });

    if (!background_equirect_->IsValid()) {
        return std::unexpected("Unable to create equirect background pass program");
    }

    return {};
}

auto GLBackgroundPass::Initialize() -> std::expected<void, std::string> {
    if (auto res = GenerateVAOs(); !res.has_value()) return res;
    if (auto res = InitializeBackground2D(); !res.has_value()) return res;
    if (auto res = InitializeBackgroundCube(); !res.has_value()) return res;
    if (auto res = InitializeBackgroundEquirect(); !res.has_value()) return res;

    return {};
}

auto GLBackgroundPass::Render(const std::shared_ptr<Texture>& background) const -> void {
    if (background == nullptr) {
        Logger::Log(LogLevel::Warning,
            "Attempting to present background using a null texture"
        );
        return;
    }

    VGLX_ASSERT(
        background->GetType() == Texture::Type::Texture2D ||
        background->GetType() == Texture::Type::CubeTexture,
        "Background texture must be of type Texture2D or CubeTexture"
    );

    if (background->GetType() == Texture::Type::Texture2D) {
        if (background->mapping == Texture::Mapping::UV) {
            glUseProgram(background_2d_->ProgramId());
            glBindVertexArray(vao_[0]);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        if (background->mapping == Texture::Mapping::Equirectangular) {
            glUseProgram(background_equirect_->ProgramId());
            glBindVertexArray(vao_[1]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    if (background->GetType() == Texture::Type::CubeTexture) {
        glUseProgram(background_cube_->ProgramId());
        glBindVertexArray(vao_[1]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

GLBackgroundPass::~GLBackgroundPass() {
    if (vao_[0]) glDeleteVertexArrays(2, vao_.data());
    if (vbo_) glDeleteBuffers(1, &vbo_);
}

}
