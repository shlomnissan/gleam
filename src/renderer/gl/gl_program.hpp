/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "renderer/gl/gl_uniform.hpp"

#include <array>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <memory>

#include <glad/glad.h>

namespace vglx {

// Forward declarations
enum class ShaderType;
struct ShaderInfo;
class Color;

constexpr auto uniforms_len = static_cast<int>(Uniform::KnownUniformsLength);

class GLProgram {
public:
    explicit GLProgram(const std::vector<ShaderInfo>& shaders);

    GLProgram(const GLProgram&) = delete;
    GLProgram(GLProgram&&) = delete;
    GLProgram& operator=(const GLProgram&) = delete;
    GLProgram& operator=(GLProgram&&) = delete;

    auto UpdateUniforms() -> void;

    auto IsValid() const { return !has_errors_ && program_ > 0; }

    auto Id() const { return program_; }

    auto SetUniform(const std::string& name, const void* v) -> void;

    auto SetUniform(Uniform uniform, const void* v) -> void;

    auto SetUniform(Uniform uniform, const Color* color) -> void;

    ~GLProgram();

private:
    std::unordered_map<std::string, GLUniform> unknown_uniforms_ {};

    std::array<std::unique_ptr<GLUniform>, uniforms_len> uniforms_ {nullptr};

    GLuint program_ {0};

    bool has_errors_ {false};

    auto BindVertexAttributeLocations() const -> void;

    auto GetUniformLoc(std::string_view name) const -> int;

    auto ProcessUniforms() -> void;

    auto ProcessUniformBlocks() -> void;

    auto CheckProgramLinkStatus() const -> bool;

    auto CheckShaderCompileStatus(GLuint shader_id) const -> bool;

    auto GetShaderType(ShaderType type) const -> GLuint;
};

}