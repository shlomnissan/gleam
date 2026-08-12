/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/core/disposable.hpp"
#include "vglx/geometries/geometry.hpp"
#include "vglx/scene/instanced_mesh.hpp"

#include "renderer/gl/gl_buffers.hpp"
#include "renderer/gl/gl_program.hpp"

#include <expected>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <glad/glad.h>

namespace vglx {

class GLBindingState {
public:
    explicit GLBindingState(GLBuffers& buffers) : buffers_(buffers) {}

    GLBindingState(const GLBindingState&) = delete;
    GLBindingState(GLBindingState&&) = delete;
    auto operator=(const GLBindingState&) -> GLBindingState& = delete;
    auto operator=(GLBindingState&&) -> GLBindingState& = delete;

    [[nodiscard]] auto Bind(Geometry& geometry, const GLProgram& program) -> GLuint;

    [[nodiscard]] auto Bind(InstancedMesh& instanced_mesh, const GLProgram& program) -> GLuint;

    auto Reset() { current_vao_ = 0; }

    ~GLBindingState();

private:
    struct Entry {
        GLuint program_id;
        GLuint ebo;
        GLuint vao;
    };

    std::shared_ptr<bool> alive_ { std::make_shared<bool>(true) };

    std::unordered_map<std::string, std::vector<Entry>> cache_ {};

    GLBuffers& buffers_;

    GLuint current_vao_ {0};

    auto GetEntry(
        const std::string& key,
        Geometry& geometry,
        const GLProgram& program,
        InstancedMesh* instanced_mesh = nullptr
    ) -> Entry*;

    auto CreateEntry(
        const std::string& key,
        Geometry& geometry,
        const GLProgram& program,
        InstancedMesh* instanced_mesh = nullptr
    ) -> std::expected<Entry, std::string>;

    auto RegisterEviction(Disposable& disposable, const std::string& key) -> void;
};

}
