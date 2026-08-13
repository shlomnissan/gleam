/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "renderer/gl/gl_binding_state.hpp"

#include "vglx/geometries/buffer_attribute.hpp"

#include "utilities/logger.hpp"

#include <format>
#include <utility>

namespace vglx {

namespace {

struct AttributeWithLocation {
    GLint location {0};
    std::shared_ptr<BufferAttribute> attribute {nullptr};
};

}

auto GLBindingState::Bind(Geometry& geometry, const GLProgram& program) -> GLuint {
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
        auto name = geometry.Name().empty() ? key : geometry.Name();
        Logger::Log(LogLevel::Error, "Failed to bind geometry {}. {}", name, entry.error());
        return 0;
    }

    auto [it, inserted] = cache_.try_emplace(key);
    if (inserted) {
        RegisterEviction(geometry, key);
    }

    it->second.geometry_uuid = geometry.UUID();
    it->second.entries.emplace_back(*entry);

    return entry->vao;
}

auto GLBindingState::Bind(InstancedMesh& instanced_mesh, const GLProgram& program) -> GLuint {
    const auto& key = instanced_mesh.UUID();

    auto geometry = instanced_mesh.GetGeometry();
    if (geometry == nullptr || geometry->Disposed()) {
        auto name = instanced_mesh.Name().empty() ? key : instanced_mesh.Name();
        Logger::Log(LogLevel::Error, "Failed to bind instanced mesh {}. Missing or disposed geometry", name);
        return 0;
    }

    auto transforms_attribute = instanced_mesh.GetInstanceAttribute(BufferAttribute::kInstanceTransform);
    if (transforms_attribute == nullptr || transforms_attribute->Disposed()) {
        auto name = instanced_mesh.Name().empty() ? key : instanced_mesh.Name();
        Logger::Log(LogLevel::Error, "Failed to bind instanced mesh {}. Missing or disposed transforms buffer attribute", name);
        return 0;
    }

    if (auto existing = GetEntry(key, *geometry, program, &instanced_mesh)) {
        return existing->vao;
    }

    auto entry = CreateEntry(key, *geometry, program, &instanced_mesh);
    if (!entry.has_value()) {
        auto name = instanced_mesh.Name().empty() ? key : instanced_mesh.Name();
        Logger::Log(LogLevel::Error, "Failed to bind instanced mesh {}. {}", name, entry.error());
        return 0;
    }

    auto [it, inserted] = cache_.try_emplace(key);
    if (inserted) {
        // The mesh itself is not disposable. Its VAOs are evicted through the
        // instance transform attribute, which is created in the mesh's
        // constructor and owned for the mesh's lifetime, so its disposal is
        // the earliest moment the instance buffers can die.
        RegisterEviction(*transforms_attribute, key);
    }

    it->second.geometry_uuid = geometry->UUID();
    it->second.entries.emplace_back(*entry);

    return entry->vao;
}

auto GLBindingState::RegisterEviction(Disposable& disposable, const std::string& key) -> void {
    disposable.OnDispose([this, alive = std::weak_ptr(alive_), key](Disposable*) {
        if (alive.expired()) return;
        if (auto it = cache_.find(key); it != cache_.end()) {
            for (const auto& entry : it->second.entries) {
                if (entry.vao == current_vao_) current_vao_ = 0;
                glDeleteVertexArrays(1, &entry.vao);
            }
            cache_.erase(it);
        }
    });
}

auto GLBindingState::GetEntry(
    const std::string& key,
    Geometry& geometry,
    const GLProgram& program,
    InstancedMesh* instanced_mesh
) -> Entry* {
    Entry* found {nullptr};

    if (auto it = cache_.find(key); it != cache_.end()) {
        auto& bucket = it->second;

        if (instanced_mesh != nullptr && bucket.geometry_uuid != geometry.UUID()) {
            for (const auto& e : bucket.entries) {
                if (e.vao == current_vao_) current_vao_ = 0;
                glDeleteVertexArrays(1, &e.vao);
            }
            bucket.entries.clear();
            return nullptr;
        }

        for (auto& e : bucket.entries) {
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

        if (instanced_mesh != nullptr) {
            for (const auto& attribute : instanced_mesh->GetInstanceAttributes()) {
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
    Geometry& geometry,
    const GLProgram& program,
    InstancedMesh* instanced_mesh
) -> std::expected<Entry, std::string> {
    if (!program.IsValid()) {
        return std::unexpected(std::format("Program ({}) is invalid", program.ProgramId()));
    }

    if (program.GetVertexAttributeLocations().empty()) {
        return std::unexpected(std::format("Program ({}) has no active vertex attributes", program.ProgramId()));
    }

    if (geometry.GetAttribute(BufferAttribute::kPosition) == nullptr) {
        return std::unexpected("Geometry missing vertex positions");
    }

    auto attributes = std::vector<AttributeWithLocation> {};
    auto missing_attributes = std::string {};
    for (const auto& a : program.GetVertexAttributeLocations()) {
        if (auto attr = geometry.GetAttribute(a.name)) {
            attributes.emplace_back(a.location, std::move(attr));
            continue;
        }

        if (instanced_mesh != nullptr) {
            if (auto attr = instanced_mesh->GetInstanceAttribute(a.name)) {
                attributes.emplace_back(a.location, std::move(attr));
                continue;
            }
        }

        missing_attributes.append(a.name + ", ");
    }

    if (!missing_attributes.empty()) {
        missing_attributes.resize(missing_attributes.size() - 2);
        return std::unexpected(std::format("Missing program attributes ({})", missing_attributes));
    }

    auto entry = Entry {.program_id = program.ProgramId()};
    glGenVertexArrays(1, &entry.vao);
    glBindVertexArray(entry.vao);
    current_vao_ = entry.vao;

    for (const auto& a : attributes) {
        auto loc = a.location;
        auto& attribute = a.attribute;

        auto vbo = buffers_.GetVertexBuffer(*attribute);
        if (vbo == 0) {
            glDeleteVertexArrays(1, &entry.vao);
            current_vao_ = 0;
            return std::unexpected(std::format("Failed to create a buffer for attribute ({})", attribute->name));
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        if (attribute->format == BufferAttribute::Format::Float32x16) {
            for (auto i = 0; i < 4; ++i) {
                glVertexAttribPointer(loc + i, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(GLfloat), reinterpret_cast<void*>(i * 4 * sizeof(GLfloat)));
                glEnableVertexAttribArray(loc + i);
                if (attribute->rate == BufferAttribute::Rate::Instance) glVertexAttribDivisor(loc + i, 1);
            }
            continue;
        }

        glVertexAttribPointer(loc, attribute->Components(), GL_FLOAT, GL_FALSE, 0, nullptr);
        if (attribute->rate == BufferAttribute::Rate::Instance) {
            glVertexAttribDivisor(loc, 1);
        }
        glEnableVertexAttribArray(loc);
    }

    auto ebo = buffers_.GetIndexBuffer(geometry);
    if (ebo) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        entry.ebo = ebo;
    }

    return entry;
}

GLBindingState::~GLBindingState() {
    for (auto& [_, bucket] : cache_) {
        for (auto& entry : bucket.entries) {
            glDeleteVertexArrays(1, &entry.vao);
        }
    }
}

}
