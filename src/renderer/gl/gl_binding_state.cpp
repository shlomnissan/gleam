/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "renderer/gl/gl_binding_state.hpp"

#include "vglx/geometries/buffer_attribute.hpp"

#include "utilities/logger.hpp"

#include <algorithm>
#include <string>

#include <glad/glad.h>

namespace vglx {

namespace {

struct AttributeWithLocation {
    GLint location {0};
    std::shared_ptr<BufferAttribute> attribute {nullptr};
};

}

auto GLBindingState::Bind(Geometry2& geometry, const GLProgram& program) -> GLuint {
    if (geometry.Disposed()) {
        auto name = geometry.Name().empty() ? geometry.UUID() : geometry.Name();
        Logger::Log(LogLevel::Error, "Failed to bind geometry {}. Geometry marked as disposed", name);
        return 0;
    }

    Entry* entry {nullptr};
    if (auto it = cache_.find(geometry.UUID()); it != cache_.end()) {
        auto& entries = it->second;
        for (auto& e : entries) {
            if (e.program_id == program.ProgramId()) {
                entry = &e;
                break;
            }
        }
    }

    if (entry != nullptr) {
        if (entry->vao != current_vao_) {
            glBindVertexArray(entry->vao);
            current_vao_ = entry->vao;
        }
        for (const auto& attribute : geometry.GetAttributes()) {
            buffers_.GetVertexBuffer(*attribute);
        }

        auto ebo = buffers_.GetIndexBuffer(geometry);
        if (ebo != entry->ebo) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            entry->ebo = ebo;
        }

        return entry->vao;
    }

    // TODO: implement creation

    return 0;
}

auto GLBindingState::Bind(InstancedMesh2& instanced_mesh, const GLProgram& program) -> GLuint {
    // TODO: implement
    return 0;
}

GLBindingState::~GLBindingState() {
    // TODO: implement
}

}
