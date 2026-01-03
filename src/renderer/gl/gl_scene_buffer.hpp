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

    [[nodiscard]] auto Initialize() -> std::expected<void, std::string>;

    auto Begin() const -> void;

    auto End() const -> void;

    ~GLSceneBuffer();

private:
    friend class GLPresentPass;
    class Impl;
    std::unique_ptr<Impl> impl_;

    [[nodiscard]] auto GetResolvedColorTexture() const -> unsigned int;
};

}