/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "gl_present_pass.hpp"

#include "core/shader_library.hpp"
#include "renderer/gl/gl_scene_buffer.hpp"

#include "shaders/internal/headers/present_pass_vert.h"
#include "shaders/internal/headers/present_pass_frag.h"

#include <vector>

namespace vglx {

auto GLPresentPass::Initialize() -> std::expected<void, std::string> {
    auto sources = std::vector<ShaderInfo> {
        {.type = ShaderType::kVertexShader, .source = _SHADER_present_pass_vert},
        {.type = ShaderType::kFragmentShader, .source = _SHADER_present_pass_frag}
    };

    program_ = std::make_unique<GLProgram>(sources);
    if (!program_->IsValid()) {
        return std::unexpected("Unable to create present pass program");
    }

    glGenVertexArrays(1, &vao_);
    if (vao_ == 0) {
        return std::unexpected("Failed to generate present pass vertex array object");
    }

    return {};
}

auto GLPresentPass::Present(const GLSceneBuffer& scene_buffer) const -> void {
    glUseProgram(program_->Id());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, scene_buffer.GetResolvedColorTexture());

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

GLPresentPass::~GLPresentPass() {
    if (vao_) glDeleteVertexArrays(1, &vao_);
}

}
