/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/geometries/geometry2.hpp"
#include "vglx/scene/instanced_mesh2.hpp"

#include "renderer/gl/gl_buffers.hpp"
#include "renderer/gl/gl_program.hpp"

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

    [[nodiscard]] auto Bind(Geometry2& geometry2, const GLProgram& program) -> GLuint;

    [[nodiscard]] auto Bind(InstancedMesh2& instanced_mesh, const GLProgram& program) -> GLuint;

    auto Reset() { current_vao_ = 0; }

    ~GLBindingState();

private:
    struct Entry {
        uint32_t layout_version;
        uint32_t instance_layout_version;
        uint32_t index_version;
        GLuint program_id;
        GLuint vao;
    };

    std::shared_ptr<bool> alive_ { std::make_shared<bool>(true) };

    std::unordered_map<std::string, std::vector<Entry>> cache_ {};

    GLBuffers& buffers_;

    GLuint current_vao_ {0};
};

}
