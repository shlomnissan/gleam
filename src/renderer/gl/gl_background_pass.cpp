/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "renderer/gl/gl_background_pass.hpp"

#include "core/shader_library.hpp"

#include "renderer/gl/gl_uniform.hpp"
#include "shaders/internal/headers/background_pass_frag.h"
#include "shaders/internal/headers/background_pass_vert.h"
#include "utilities/assert.hpp"
#include "utilities/logger.hpp"

#include <vector>

namespace vglx {

auto GLBackgroundPass::Initialize() -> std::expected<void, std::string> {
    auto sources = std::vector<ShaderInfo> {
        {.type = ShaderType::kVertexShader, .source = _SHADER_background_pass_vert},
        {.type = ShaderType::kFragmentShader, .source = _SHADER_background_pass_frag}
    };

    program_ = std::make_unique<GLProgram>(sources);
    if (!program_->IsValid()) {
        return std::unexpected("Unable to create background pass program");
    }

    glGenVertexArrays(1, &vao_);
    if (vao_ == 0) {
        return std::unexpected("Failed to generate background pass vertex array object");
    }

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
        background->GetType() == Texture::Type::Texture2D,
        "Background texture must be of type Texture2D"
    );

    glUseProgram(program_->Id());

    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

GLBackgroundPass::~GLBackgroundPass() {
    if (vao_) glDeleteVertexArrays(1, &vao_);
}

}