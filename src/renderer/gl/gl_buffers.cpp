/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "renderer/gl/gl_buffers.hpp"

#include "utilities/logger.hpp"

namespace vglx {

namespace {

auto update_vertex_buffer_object(GLuint vbo, const BufferAttribute& attribute) -> void {
    const auto& data = attribute.GetData();
    const auto usage = attribute.rate == BufferAttribute::Rate::Instance
        ? GL_DYNAMIC_DRAW
        : GL_STATIC_DRAW;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        data.size() * sizeof(GLfloat),
        data.data(),
        usage
    );
}

auto update_element_buffer_object(GLuint ebo, const Geometry& geometry) -> void {
    const auto& data = geometry.GetIndexData();

    // Index data is deliberately uploaded through GL_ARRAY_BUFFER: binding
    // GL_ELEMENT_ARRAY_BUFFER here would overwrite the element binding of
    // whatever VAO is currently bound. Buffer objects aren't typed, so the
    // target only matters when GLBindingState attaches the buffer to a VAO.
    glBindBuffer(GL_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ARRAY_BUFFER,
        data.size() * sizeof(uint32_t),
        data.data(),
        GL_STATIC_DRAW
    );
}

}

auto GLBuffers::GetVertexBuffer(BufferAttribute& attribute) -> GLuint {
    if (attribute.Disposed()) {
        Logger::Log(
            LogLevel::Error,
            "Failed to get vertex buffer for attribute {}. The attribute has been disposed",
            attribute.name
        );
        return 0;
    }

    if (!attribute.IsValid()) {
        Logger::Log(
            LogLevel::Error,
            "Failed to get vertex buffer for attribute {}. Invalid attribute, missing name or data",
            attribute.name
        );
        return 0;
    }

    if (auto it = cache_.find(attribute.UUID()); it != cache_.end()) {
        if (auto version = attribute.GetVersion(); it->second.version != version) {
            update_vertex_buffer_object(it->second.buffer_id, attribute);
            it->second.version = version;
        }
        return it->second.buffer_id;
    }

    auto buffer_id = GLuint {0};
    glGenBuffers(1, &buffer_id);
    update_vertex_buffer_object(buffer_id, attribute);

    auto entry = Entry {
        .version = attribute.GetVersion(),
        .buffer_id = buffer_id
    };

    RegisterEviction(attribute, attribute.UUID());

    cache_.emplace(attribute.UUID(), entry);

    return entry.buffer_id;
}

auto GLBuffers::GetIndexBuffer(Geometry& geometry) -> GLuint {
    const auto& geometry_name = geometry.DisplayName();

    if (geometry.Disposed()) {
        Logger::Log(
            LogLevel::Error,
            "Failed to get index buffer for geometry {}. The geometry has been disposed",
            geometry_name
        );
        return 0;
    }

    if (geometry.GetIndexData().empty()) return 0;

    if (geometry.GetMaxIndex() >= geometry.VertexCount()) {
        Logger::Log(
            LogLevel::Error,
            "Invalid index buffer for geometry {}. Max index element exceeds vertex count",
            geometry_name
        );
        return 0;
    }

    if (auto it = cache_.find(geometry.UUID()); it != cache_.end()) {
        if (auto version = geometry.GetIndexVersion(); it->second.version != version) {
            update_element_buffer_object(it->second.buffer_id, geometry);
            it->second.version = version;
        }
        return it->second.buffer_id;
    }

    auto buffer_id = GLuint {0};
    glGenBuffers(1, &buffer_id);
    update_element_buffer_object(buffer_id, geometry);

    auto entry = Entry {
        .version = geometry.GetIndexVersion(),
        .buffer_id = buffer_id
    };

    RegisterEviction(geometry, geometry.UUID());

    cache_.emplace(geometry.UUID(), entry);

    return entry.buffer_id;
}

auto GLBuffers::RegisterEviction(Disposable& disposable, const std::string& uuid) -> void {
    disposable.OnDispose([this, alive = std::weak_ptr(alive_), uuid](const std::string&) {
        if (alive.expired()) return;
        if (auto it = cache_.find(uuid); it != cache_.end()) {
            glDeleteBuffers(1, &it->second.buffer_id);
            cache_.erase(it);
        }
    });
}

GLBuffers::~GLBuffers() {
    for (const auto& [key, value] : cache_) {
        glDeleteBuffers(1, &value.buffer_id);
    }
}

}
