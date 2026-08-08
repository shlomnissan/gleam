/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/geometries/buffer_attribute.hpp"
#include "vglx/geometries/geometry2.hpp"

#include <cstdint>
#include <memory>
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

    [[nodiscard]] auto GetVertexBuffer(BufferAttribute& attribute) -> GLuint;

    [[nodiscard]] auto GetIndexBuffer(Geometry2& geometry) -> GLuint;

    ~GLBuffers();

private:
    struct Entry { uint32_t version; GLuint buffer_id; };

    std::shared_ptr<bool> alive_ { std::make_shared<bool>(true) };

    std::unordered_map<std::string, Entry> cache_ {};

    auto RegisterEviction(Disposable& disposable, const std::string& uuid) -> void;
};

}
