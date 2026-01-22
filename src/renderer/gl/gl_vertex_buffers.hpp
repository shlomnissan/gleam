/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <array>
#include <memory>
#include <unordered_map>
#include <vector>

#include <glad/glad.h>

namespace vglx {

class Geometry;
class InstancedMesh;

class GLVertexBuffers {
public:
    GLVertexBuffers() = default;

    // delete copy constructor and assignment operator
    GLVertexBuffers(const GLVertexBuffers&) = delete;
    GLVertexBuffers& operator=(const GLVertexBuffers&) = delete;

    // delete move constructor and assignment operator
    GLVertexBuffers(GLVertexBuffers&&) = delete;
    GLVertexBuffers& operator=(GLVertexBuffers&&) = delete;

    auto Bind(const std::shared_ptr<Geometry>& geometry) -> void;

    auto BindInstancedMesh(InstancedMesh* mesh) -> void;

    auto Reset() -> void;

    ~GLVertexBuffers();

private:
    std::vector<std::weak_ptr<Geometry>> geometries_;

    std::unordered_map<GLuint, std::array<GLuint, 4>> bindings_;

    GLuint current_vao_ {0};

    auto GenerateBuffers(Geometry* geometry) -> void;
};

}