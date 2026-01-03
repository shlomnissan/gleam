/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <memory>

namespace vglx {

class GLSceneBuffer;

class GLPresentPass {
public:
    GLPresentPass();

    // Non-copyable
    GLPresentPass(const GLPresentPass&) = delete;
    auto operator=(const GLPresentPass&) -> GLPresentPass& = delete;

    // Non-moveable
    GLPresentPass(GLPresentPass&&) = delete;
    auto operator=(GLPresentPass&&) -> GLPresentPass& = delete;

    auto Present(GLSceneBuffer& scene_buffer) -> void;

    ~GLPresentPass();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}