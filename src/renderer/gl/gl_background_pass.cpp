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
#include "shaders/internal/headers/background_frag.h"
#include "shaders/internal/headers/background_vert.h"
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

auto GLBackgroundPass::InitializeBackground2D() -> std::expected<void, std::string> {
    auto sources = std::vector<ShaderInfo> {
        {.type = ShaderType::kVertexShader, .source = _SHADER_background_vert},
        {.type = ShaderType::kFragmentShader, .source = _SHADER_background_frag}
    };

    background_2d_ = std::make_unique<GLProgram>(sources);
    if (!background_2d_->IsValid()) {
        return std::unexpected("Unable to create background pass program");
    }

    return {};
}

auto GLBackgroundPass::InitializeBackgroundCube() -> std::expected<void, std::string> {
    auto sources = std::vector<ShaderInfo> {
        {.type = ShaderType::kVertexShader, .source = _SHADER_background_cube_vert},
        {.type = ShaderType::kFragmentShader, .source = _SHADER_background_cube_frag}
    };

    background_cube_ = std::make_unique<GLProgram>(sources);
    if (!background_cube_->IsValid()) {
        return std::unexpected("Unable to create background cube pass program");
    }

    GLuint vbo {0};
    glGenBuffers(1, &vbo);

    if (vbo == 0) {
        return std::unexpected("Failed to generate vertex buffer for background cube");
    }

    glBindVertexArray(vao_[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
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

auto GLBackgroundPass::Initialize() -> std::expected<void, std::string> {
    glGenVertexArrays(2, vao_.data());
    if (vao_[0] == 0 || vao_[1] == 0) {
        return std::unexpected("Failed to generate background pass vertex array object");
    }

    if (auto res = InitializeBackground2D(); !res.has_value()) return res;
    if (auto res = InitializeBackgroundCube(); !res.has_value()) return res;

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
        glUseProgram(background_2d_->Id());
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(vao_[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    if (background->GetType() == Texture::Type::CubeTexture) {
        glUseProgram(background_cube_->Id());
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(vao_[1]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

GLBackgroundPass::~GLBackgroundPass() {
    if (vao_[0]) glDeleteVertexArrays(2, vao_.data());
}

}