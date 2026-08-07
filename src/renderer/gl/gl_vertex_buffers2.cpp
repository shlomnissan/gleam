/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "renderer/gl/gl_vertex_buffers2.hpp"

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

auto generate_vertex_buffer_object(const BufferAttribute& attribute) -> GLuint {
    auto vbo = GLuint {0};
    glGenBuffers(1, &vbo);
    update_vertex_buffer_object(vbo, attribute);
    return vbo;
}

}

auto GLVertexBuffers2::GetVertexBuffer(BufferAttribute& attribute) -> std::optional<GLuint> {
    if (attribute.Disposed()) return std::nullopt;

    if (!attribute.IsValid()) {
        Logger::Log(
            LogLevel::Error,
            "Failed to get vertex buffer for an invalid attribute {}. Invalid attribute, missing name or data",
            attribute.name
        );
        return std::nullopt;
    }

    if (auto it = cache_.find(attribute.UUID()); it != cache_.end()) {
        if (auto version = attribute.GetVersion(); it->second.version != version) {
            update_vertex_buffer_object(it->second.vbo, attribute);
            it->second.version = version;
        }
        return it->second.vbo;
    }

    auto entry = Entry {
        .version = attribute.GetVersion(),
        .vbo = generate_vertex_buffer_object(attribute)
    };

    attribute.OnDispose([this, alive = std::weak_ptr(alive_), uuid = attribute.UUID()](Disposable*) {
        if (alive.expired()) return;
        if (auto it = cache_.find(uuid); it != cache_.end()) {
            glDeleteBuffers(1, &it->second.vbo);
            cache_.erase(it);
        }
    });

    cache_.emplace(attribute.UUID(), entry);

    return entry.vbo;
}

GLVertexBuffers2::~GLVertexBuffers2() {
    for (const auto& [key, value] : cache_) {
        glDeleteBuffers(1, &value.vbo);
    }
}

}
