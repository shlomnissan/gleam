/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/core/render_target.hpp"

#include "vglx/textures/texture.hpp"

namespace vglx {

RenderTarget::RenderTarget(const Parameters& params)
  : width(params.width),
    height(params.height),
    format(params.format),
    has_depth(params.has_depth),
    enable_readback(params.enable_readback)
{
    if (enable_readback) {
        const auto bpp = bytes_per_pixel(format);
        color_data_.resize(static_cast<size_t>(width) * height * bpp);
    }
}

auto RenderTarget::ReadColorData() const -> std::span<const uint8_t> {
    if (!enable_readback || !has_readback_) {
        return {};
    }
    return std::span<const std::uint8_t>(color_data_.data(), color_data_.size());
}

}