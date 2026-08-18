/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/core/disposable.hpp"
#include "vglx/geometries/buffer_attribute.hpp"
#include "vglx/geometries/geometry.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include <glad/glad.h>

namespace vglx {

class GLBuffers {
public:
    GLBuffers() = default;

    GLBuffers(const GLBuffers&) = delete;
    GLBuffers(GLBuffers&&) = delete;

    auto operator=(const GLBuffers&) -> GLBuffers& = delete;
    auto operator=(GLBuffers&&) -> GLBuffers& = delete;

    auto GetVertexBuffer(BufferAttribute& attribute) -> GLuint;

    // Returns nullopt if the geometry is disposed or its index data is
    // invalid, 0 if the geometry is non-indexed, and a buffer id otherwise.
    auto GetIndexBuffer(Geometry& geometry) -> std::optional<GLuint>;

    ~GLBuffers();

private:
    struct Entry { uint32_t version; GLuint buffer_id; };

    std::shared_ptr<bool> alive_ { std::make_shared<bool>(true) };

    std::unordered_map<std::string, Entry> cache_ {};

    auto RegisterEviction(Disposable& disposable, const std::string& uuid) -> void;
};

}
