/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "gl_present_pass.hpp"

#include "renderer/gl/gl_scene_buffer.hpp"

namespace vglx {

struct GLPresentPass::Impl {};

GLPresentPass::GLPresentPass()
  : impl_(std::make_unique<GLPresentPass::Impl>()) {}

auto GLPresentPass::Present(GLSceneBuffer& scene_buffer) -> void {}

GLPresentPass::~GLPresentPass() = default;

}