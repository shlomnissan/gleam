/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <expected>
#include <memory>
#include <string>

namespace vglx {

class GLSceneBuffer {
public:
    struct Parameters {
        int framebuffer_width;
        int framebuffer_height;
        int sample_count;
    };

    explicit GLSceneBuffer(const Parameters& params);

    // Non-copyable
    GLSceneBuffer(const GLSceneBuffer&) = delete;
    auto operator=(const GLSceneBuffer&) -> GLSceneBuffer& = delete;

    // Non-moveable
    GLSceneBuffer(GLSceneBuffer&&) = delete;
    auto operator=(GLSceneBuffer&&) -> GLSceneBuffer& = delete;

    [[nodiscard]] auto Init() -> std::expected<void, std::string>;

    auto GetResolvedColorTexture() const -> unsigned int;

    auto Begin() -> void;

    auto End() -> void;

    ~GLSceneBuffer();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}