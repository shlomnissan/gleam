/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "renderer/gl/gl_program.hpp"

#include "vglx/geometries/geometry.hpp"

#include "core/shader_library.hpp"
#include "utilities/logger.hpp"
#include "utilities/assert.hpp"

#include <utility>

namespace vglx {

namespace {

const auto VertexAttributesMap = std::unordered_map<std::string, Geometry::VertexAttributeType> {
    {"a_Position", Geometry::VertexAttributeType::Position},
    {"a_Normal", Geometry::VertexAttributeType::Normal},
    {"a_TexCoord", Geometry::VertexAttributeType::UV},
    {"a_Tangent", Geometry::VertexAttributeType::Tangent},
    {"a_Color", Geometry::VertexAttributeType::Color},
    {"a_InstanceColor", Geometry::VertexAttributeType::InstanceColor},
    {"a_InstanceTransform", Geometry::VertexAttributeType::InstanceTransform},
};

}

GLProgram::GLProgram(const std::vector<ShaderInfo>& shaders) {
    program_ = glCreateProgram();

    auto compilation_error = false;
    for (const auto& shader_info : shaders) {
        auto shader_id = glCreateShader(GetShaderType(shader_info.type));
        auto data = shader_info.source.data();

        glShaderSource(shader_id, 1, &data, nullptr);
        glCompileShader(shader_id);

        if (!CheckShaderCompileStatus(shader_id)) {
            compilation_error = true;
            break;
        }

        glAttachShader(program_, shader_id);
        glDeleteShader(shader_id);
    }

    if (compilation_error) {
        has_errors_ = true;
        return;
    }

    BindVertexAttributeLocations();

    glLinkProgram(program_);
    if (!CheckProgramLinkStatus()) {
        has_errors_ = true;
        return;
    }

    ProcessUniforms();
    ProcessUniformBlocks();
}

auto GLProgram::UpdateUniforms() -> void {
    for (auto& [_, uniform] : unknown_uniforms_) {
        uniform.UploadIfNeeded();
    }

    for (auto& uniform : uniforms_) {
        if (uniform != nullptr) uniform->UploadIfNeeded();
    }
}

auto GLProgram::SetUnknownUniform(const std::string& name, const void* v) -> void {
    unknown_uniforms_.at(name).SetValue(v);
}

auto GLProgram::SetUniform(Uniform uniform, const void* v) -> void {
    auto i = static_cast<int>(uniform);
    if (uniforms_[i] != nullptr) {
        uniforms_[i]->SetValue(v);
    }
}

auto GLProgram::BindVertexAttributeLocations() const -> void {
    for (auto& [attr_name, attr_type] : VertexAttributesMap) {
        glBindAttribLocation(
            program_,
            static_cast<int>(attr_type),
            attr_name.data()
        );
    }
}

auto GLProgram::GetUniformLoc(std::string_view name) const -> int {
    return glGetUniformLocation(program_, name.data());
}

auto GLProgram::ProcessUniforms() -> void {
    auto n_active_uniforms = GLint {0};
    glGetProgramiv(program_, GL_ACTIVE_UNIFORMS, &n_active_uniforms);

    auto max_name_length = GLsizei {0};
    glGetProgramiv(program_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_length);
    auto buffer = std::string {"", static_cast<size_t>(max_name_length)};

    for (auto i = 0; i < n_active_uniforms; ++i) {
        auto length = GLsizei {};
        auto size_unused = GLint {};
        auto type = GLenum {};
        glGetActiveUniform(
            program_, i,
            max_name_length,
            &length,
            &size_unused,
            &type,
            buffer.data()
        );

        auto name = std::string(buffer.data(), length);
        auto idx = get_uniform_loc(name);
        if (idx != -1) {
            uniforms_[idx] = std::make_unique<GLUniform>(name, GetUniformLoc(name), type);
        } else {
            unknown_uniforms_.try_emplace(name, name, GetUniformLoc(name), type);
        }
    }
}

auto GLProgram::ProcessUniformBlocks() -> void {
    auto n_active_uniforms = GLint {0};
    glGetProgramiv(program_, GL_ACTIVE_UNIFORM_BLOCKS, &n_active_uniforms);

    GLint max_name_length = 0;
    glGetProgramiv(program_, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &max_name_length);
    auto buffer = std::string {"", static_cast<size_t>(max_name_length)};

    for (GLint i = 0; i < n_active_uniforms; ++i) {
        auto length = GLsizei {};
        glGetActiveUniformBlockName(program_, i, max_name_length, &length, buffer.data());
        auto name = std::string(buffer.data(), length);
        auto idx = get_uniform_block_loc(name);
        if (idx == -1) {
            Logger::Log(LogLevel::Error, "Unknown uniform block {}", name);
            continue;
        }
        glUniformBlockBinding(program_, i, idx);
    }
}

auto GLProgram::CheckShaderCompileStatus(GLuint shader_id) const -> bool {
    auto success = 0;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        auto length = 0;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
        auto buffer = std::string {"", static_cast<size_t>(length)};
        glGetShaderInfoLog(shader_id, length, nullptr, buffer.data());
        Logger::Log(LogLevel::Error, "Shader compilation error {}", buffer);
    }
    return success;
}

auto GLProgram::CheckProgramLinkStatus() const -> bool {
    auto success = 0;
    glGetProgramiv(program_, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        auto length = 0;
        glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &length);
        auto buffer = std::string {"", static_cast<size_t>(length)};
        glGetProgramInfoLog(program_, length, nullptr, buffer.data());
        Logger::Log(LogLevel::Error, "Shader program link error {}", buffer);
    }
    return success;
}

auto GLProgram::GetShaderType(ShaderType type) const -> GLuint {
    switch(type) {
        case ShaderType::kVertexShader:
            return GL_VERTEX_SHADER;
        case ShaderType::kFragmentShader:
            return GL_FRAGMENT_SHADER;
        default:
            VGLX_UNREACHABLE();
    }
}

GLProgram::~GLProgram() {
    if (program_ > 0) glDeleteProgram(program_);
}

}