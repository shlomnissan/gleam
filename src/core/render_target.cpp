/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/core/render_target.hpp"

#include "vglx/textures/image.hpp"
#include "vglx/textures/texture.hpp"

#include "utilities/logger.hpp"

#include <algorithm>

namespace vglx {

namespace {

auto validate_dimension(int value) -> int {
    if (value < 1) {
        Logger::Log(LogLevel::Error, "Invalid render target dimension {}, clamping to 1", value);
    }
    return std::max(value, 1);
}

}

RenderTarget::RenderTarget(const Parameters& params)
  : width(validate_dimension(params.width)),
    height(validate_dimension(params.height)),
    format(params.format),
    has_depth(params.has_depth),
    enable_readback(params.enable_readback)
{
    texture_ = Texture2D::Create(Image::Create({
        .width = static_cast<unsigned int>(width),
        .height = static_cast<unsigned int>(height)
    }));

    texture_->format = format;
    texture_->color_space = Texture::ColorSpace::Linear;
    texture_->wrap_s = Texture::Wrapping::ClampToEdge;
    texture_->wrap_t = Texture::Wrapping::ClampToEdge;

    OnDispose([texture = texture_](const std::string&) {
        texture->Dispose();
    });

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
