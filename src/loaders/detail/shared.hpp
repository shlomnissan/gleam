/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/math/vector2.hpp"
#include "vglx/textures/texture.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

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

struct VertexLayout {
    uint32_t stride {0};
    uint32_t position_offset {0};
    uint32_t normal_offset {0};

    std::optional<uint32_t> uv_offset;
    std::optional<uint32_t> tangent_offset;
    std::optional<uint32_t> color_offset;

    bool has_uvs {false};
    bool has_tangents {false};
    bool has_colors {false};
};

auto make_layout(
    bool has_uvs,
    bool has_colors
) -> VertexLayout;

auto generate_normals(
    std::vector<float>& vertex_data,
    std::vector<unsigned>& index_data,
    const VertexLayout& layout
) -> void;

auto generate_tangents(
    std::vector<float>& vertex_data,
    std::vector<unsigned>& index_data,
    const VertexLayout& layout
) -> void;

}