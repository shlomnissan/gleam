/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "renderer/gl/gl_scene_buffer.hpp"

namespace vglx {

struct GLSceneBuffer::Impl {
    GLSceneBuffer::Parameters params;

    explicit Impl(const GLSceneBuffer::Parameters& params) : params(params) {}
};

GLSceneBuffer::GLSceneBuffer(const Parameters& params)
  : impl_(std::make_unique<GLSceneBuffer::Impl>(params)) {}

auto GLSceneBuffer::Begin() const -> void {}

auto GLSceneBuffer::End() const -> void {}

GLSceneBuffer::~GLSceneBuffer() = default;

}