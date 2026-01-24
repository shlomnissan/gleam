/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/textures/dynamic_texture_2d.hpp"

#include "utilities/logger.hpp"

namespace vglx {

namespace {

auto mip_dimensions(const DynamicTexture2D& tex, unsigned int mip_level) {
    const auto w = std::max(1, tex.width  >> mip_level);
    const auto h = std::max(1, tex.height >> mip_level);
    return std::pair {w, h};
}

}

DynamicTexture2D::DynamicTexture2D(const Parameters& params) :
    width(params.width),
    height(params.height),
    mips(params.mips)
{
    format = params.format;
    color_space = params.color_space;
    row_alignment = RowAlignment::OneByte;
    generate_mipamps = false;
    min_filter = MinFilter::Nearest;
    mag_filter = MagFilter::Nearest;
}

auto DynamicTexture2D::UpdateSubregion(
    unsigned int mip_level,
    int x,
    int y,
    int w,
    int h,
    std::span<const std::uint8_t> data
) -> void {
    if (mip_level >= mips) {
        Logger::Log(
            LogLevel::Error,
            "UpdateSubregion failed: mip level {} is out of range (max {})",
            mip_level, mips - 1
        );
        return;
    }

    if (w <= 0 || h <= 0) {
        Logger::Log(
            LogLevel::Error,
            "UpdateSubregion failed: invalid subregion size ({} x {})",
            w, h
        );
        return;
    }

    const auto [mw, mh] = mip_dimensions(*this, mip_level);
    if (x < 0 || y < 0 || x + w > mw || y + h > mh) {
        Logger::Log(
            LogLevel::Error,
            "UpdateSubregion failed: subregion exceeds mip {} dimensions ({} x {})",
            mip_level, mw, mh
        );
        return;
    }

    const auto bpp = bytes_per_pixel(format);
    const auto expected = static_cast<std::size_t>(w) * h * bpp;
    if (static_cast<int>(data.size()) != expected) {
        Logger::Log(
            LogLevel::Error,
            "UpdateSubregion failed: data size mismatch (got {} bytes, expected {} bytes)",
            data.size(), expected
        );
        return;
    }

    pending_.emplace_back(PendingUpdate {
        .mip_level = mip_level,
        .x = x,
        .y = y,
        .w = w,
        .h = h,
        .bytes = {data.begin(), data.end()}
    });
}

}