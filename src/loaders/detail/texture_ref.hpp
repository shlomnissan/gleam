/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/math/vector2.hpp"
#include "vglx/textures/texture.hpp"

#include <string>

namespace vglx::detail {

struct TextureRef {
    std::string uri;

    Texture::MinFilter min_filter {Texture::MinFilter::Linear};
    Texture::MagFilter mag_filter {Texture::MagFilter::Linear};
    Texture::Wrapping wrap_s {Texture::Wrapping::Repeat};
    Texture::Wrapping wrap_t {Texture::Wrapping::Repeat};

    Vector2 uv_offset {0.0f, 0.0f};
    Vector2 uv_scale {1.0f, 1.0f};

    float uv_rotation {0.0f};

    [[nodiscard]] auto empty() const -> bool { return uri.empty(); }
};

}
