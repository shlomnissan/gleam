/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "gl_present_pass.hpp"

#include "core/shader_library.hpp"
#include "renderer/gl/gl_program.hpp"
#include "renderer/gl/gl_scene_buffer.hpp"

#include "shaders/internal/headers/present_pass_vert.h"
#include "shaders/internal/headers/present_pass_frag.h"

#include <glad/glad.h>

#include <vector>

namespace vglx {

struct GLPresentPass::Impl {
    GLuint vao {0};

    std::unique_ptr<GLProgram> program {nullptr};

    auto Initialize() -> std::expected<void, std::string> {
        auto sources = std::vector<ShaderInfo> {
            {.type = ShaderType::kVertexShader, .source = _SHADER_present_pass_vert},
            {.type = ShaderType::kFragmentShader, .source = _SHADER_present_pass_frag}
        };

        program = std::make_unique<GLProgram>(sources);
        if (!program->IsValid()) {
            return std::unexpected("Unable to create present pass program");
        }

        glGenVertexArrays(1, &vao);
        if (vao == 0) {
            return std::unexpected("Failed to generate present pass array buffer");
        }

        return {};
    }

    ~Impl() {
        if (vao) glDeleteVertexArrays(1, &vao);
    }
};

GLPresentPass::GLPresentPass()
  : impl_(std::make_unique<GLPresentPass::Impl>()) {}

auto GLPresentPass::Initialize() -> std::expected<void, std::string> {
    return impl_->Initialize();
}

auto GLPresentPass::Present(GLSceneBuffer& scene_buffer) -> void {}

GLPresentPass::~GLPresentPass() = default;

}