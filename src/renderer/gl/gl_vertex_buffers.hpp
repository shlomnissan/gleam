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
#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <glad/glad.h>

namespace vglx {

class GLVertexBuffers {
public:
    GLVertexBuffers() = default;

    GLVertexBuffers(const GLVertexBuffers&) = delete;
    GLVertexBuffers(GLVertexBuffers&&) = delete;
    GLVertexBuffers& operator=(const GLVertexBuffers&) = delete;
    GLVertexBuffers& operator=(GLVertexBuffers&&) = delete;

    auto Bind(const std::shared_ptr<Geometry>& geometry) -> void;

    auto BindInstancedMesh(InstancedMesh* mesh) -> void;

    auto Reset() -> void;

    ~GLVertexBuffers();

private:
    std::unordered_map<GLuint, std::array<GLuint, 4>> bindings_;

    std::vector<std::weak_ptr<Geometry>> geometries_;

    GLuint current_vao_ {0};

    auto GenerateBuffers(Geometry* geometry) -> void;
};

}