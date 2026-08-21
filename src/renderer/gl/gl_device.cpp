/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "renderer/gl/gl_device.hpp"

#include <glad/glad.h>

#include <string>

namespace vglx::gl {

// GL_EXT_texture_filter_anisotropic; core in GL 4.6 but GLAD targets 4.1 on macOS.
#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY
#define GL_MAX_TEXTURE_MAX_ANISOTROPY 0x84FF
#endif

namespace {

auto query_string(GLenum name) -> std::string {
    const auto* s = reinterpret_cast<const char*>(glGetString(name));
    return s ? std::string {s} : std::string {};
}

}

auto limits() -> const Renderer::Limits& {
    static const Renderer::Limits values = [] {
        auto l = Renderer::Limits {};
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &l.max_anisotropy);
        glGetIntegerv(GL_MAX_SAMPLES, &l.max_samples);
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &l.max_texture_units);
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &l.max_texture_size);
        glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &l.max_cube_map_size);
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &l.max_renderbuffer_size);
        return l;
    }();
    return values;
}

auto driver_info() -> const Renderer::DriverInfo& {
    static const Renderer::DriverInfo info = [] {
        auto d = Renderer::DriverInfo {};
        d.vendor = query_string(GL_VENDOR);
        d.renderer = query_string(GL_RENDERER);
        d.version = query_string(GL_VERSION);
        d.glsl_version = query_string(GL_SHADING_LANGUAGE_VERSION);
        return d;
    }();
    return info;
}

}
