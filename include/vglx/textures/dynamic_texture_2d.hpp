/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/textures/texture.hpp"

#include <cstdint>
#include <memory>
#include <span>
#include <vector>

namespace vglx {

class VGLX_EXPORT DynamicTexture2D : public Texture {
public:
    enum class Format {
        RGBA8,
        RGBA16F,
        R16F,
        R32F,
        R32UI
    };

    struct Parameters {
        int width;
        int height;
        unsigned int mips = 1;
        Format format = Format::RGBA8;
        ColorSpace color_space = ColorSpace::Linear;
    };

    const int width;
    const int height;
    const unsigned int mips;
    const Format format;

    explicit DynamicTexture2D(const Parameters& params) :
        width(params.width),
        height(params.height),
        mips(params.mips),
        format(params.format)
    {
        color_space = params.color_space;
    }

    [[nodiscard]] static auto Create(const Parameters& params)
      -> std::shared_ptr<DynamicTexture2D> {
        return std::make_shared<DynamicTexture2D>(params);
    }

    [[nodiscard]] auto GetType() const -> Texture::Type override {
        return Texture::Type::DynamicTexture2D;
    }

    auto UpdateSubregion(
        unsigned int mip_level,
        int x,
        int y,
        int w,
        int h,
        std::span<const std::uint8_t> data
    ) -> void;

private:
    friend class GLTextures;

    struct PendingUpdate {
        unsigned int mip_level;
        int x;
        int y;
        int w;
        int h;
        std::vector<std::uint8_t> bytes;
    };

    std::vector<PendingUpdate> pending_;
};

}