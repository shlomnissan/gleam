/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "renderer/gl/gl_environment.hpp"

#include "vglx/textures/texture.hpp"

#include "renderer/gl/gl_program.hpp"

namespace vglx {

namespace {

auto bind_cube_face(GLuint cube, int face, int mip, int size) {
    return;
}

auto dispose(GLEnvironmentMaps& maps) {
    if (maps.base_cube) glDeleteTextures(1, &maps.base_cube);
    if (maps.irradiance) glDeleteTextures(1, &maps.irradiance);
    if (maps.prefiltered) glDeleteTextures(1, &maps.prefiltered);

    maps.base_cube = 0;
    maps.irradiance = 0;
    maps.prefiltered = 0;
}

auto create_cube(int size, bool mips) -> GLuint {
    return 0;
}

}

auto GLEnvironment::Initialize() -> std::expected<void, std::string> {
    return {};
}

auto GLEnvironment::GetOrProcess(const std::shared_ptr<Texture>& source) -> std::optional<GLEnvironmentMaps> {
    return std::nullopt;
}

auto GLEnvironment::Process(const std::shared_ptr<Texture>& source) -> GLEnvironmentMaps {
    return GLEnvironmentMaps {};
}

auto GLEnvironment::EquirectToCube(GLuint src_equirect, GLuint dst_cube, int size) -> void {
    return;
}

GLEnvironment::~GLEnvironment() {
    for (auto& [_, map] : cache_) dispose(map);

    if (fbo_) glDeleteFramebuffers(1, &fbo_);
    if (cube_vao_) glDeleteVertexArrays(1, &cube_vao_);
    if (cube_vbo_) glDeleteBuffers(1, &cube_vbo_);
}

}