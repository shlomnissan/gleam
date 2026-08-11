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
    const auto& key = geometry.UUID();

    if (geometry.Disposed()) {
        auto name = geometry.Name().empty() ? key : geometry.Name();
        Logger::Log(LogLevel::Error, "Failed to bind geometry {}. Geometry marked as disposed", name);
        return 0;
    }

    if (auto existing = GetEntry(key, geometry, program)) {
        return existing->vao;
    }

    auto entry = CreateEntry(key, geometry, program);
    if (!entry.has_value()) {
        Logger::Log(LogLevel::Error, "{}", entry.error());
        return 0;
    }

    auto [it, inserted] = cache_.try_emplace(key);
    if (inserted) {
        geometry.OnDispose([this, alive = std::weak_ptr(alive_), key](Disposable*) {
            if (alive.expired()) return;
            if (auto it = cache_.find(key); it != cache_.end()) {
                for (const auto& entry : it->second) {
                    if (entry.vao == current_vao_) current_vao_ = 0;
                    glDeleteVertexArrays(1, &entry.vao);
                }
                cache_.erase(it);
            }
        });
    }

    it->second.emplace_back(*entry);

    return entry->vao;
}

auto GLBindingState::Bind(InstancedMesh2& instanced_mesh, const GLProgram& program) -> GLuint {
    // TODO: implement
    return 0;
}

auto GLBindingState::GetEntry(
    const std::string& key,
    Geometry2& geometry,
    const GLProgram& program,
    InstancedMesh2* mesh
) -> Entry* {
    Entry* found {nullptr};

    if (auto it = cache_.find(key); it != cache_.end()) {
        auto& entries = it->second;
        for (auto& e : entries) {
            if (e.program_id == program.ProgramId()) {
                found = &e;
                break;
            }
        }
    }

    if (found != nullptr) {
        if (found->vao != current_vao_) {
            glBindVertexArray(found->vao);
            current_vao_ = found->vao;
        }

        for (const auto& attribute : geometry.GetAttributes()) {
            buffers_.GetVertexBuffer(*attribute);
        }

        if (mesh != nullptr) {
            for (const auto& attribute : mesh->GetInstanceAttributes()) {
                buffers_.GetVertexBuffer(*attribute);
            }
        }

        auto ebo = buffers_.GetIndexBuffer(geometry);
        if (ebo != found->ebo) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            found->ebo = ebo;
        }

        return found;
    }

    return nullptr;
}

auto GLBindingState::CreateEntry(
    const std::string& key,
    Geometry2& geometry,
    const GLProgram& program,
    InstancedMesh2* mesh
) -> std::expected<Entry, std::string> {
    return std::unexpected("not implemented");
}

GLBindingState::~GLBindingState() {
    for (auto& [_, entries] : cache_) {
        for (auto& entry : entries) {
            glDeleteVertexArrays(1, &entry.vao);
        }
    }
}

}
