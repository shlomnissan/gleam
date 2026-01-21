/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/geometries/geometry.hpp"
#include "vglx/scene/instanced_mesh.hpp"

#include <array>
#include <unordered_map>

#include <glad/glad.h>

namespace vglx {

class GLVertexBuffers {
public:
    GLVertexBuffers() = default;

    // delete copy constructor and assignment operator
    GLVertexBuffers(const GLVertexBuffers&) = delete;
    GLVertexBuffers& operator=(const GLVertexBuffers&) = delete;

    // delete move constructor and assignment operator
    GLVertexBuffers(GLVertexBuffers&&) = delete;
    GLVertexBuffers& operator=(GLVertexBuffers&&) = delete;

    auto Bind(Geometry* geometry) -> void;

    auto BindInstancedMesh(InstancedMesh* mesh) -> void;

    auto Reset() -> void;

private:
    std::unordered_map<GLuint, std::array<GLuint, 4>> bindings_;

    GLuint current_vao_ {0};

    auto GenerateBuffers(Geometry* geometry) -> void;
};

}