/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/core/disposable.hpp"

#include <cstdint>

namespace vglx {

/**
 * @brief Abstract base class for texture types.
 *
 * This class is not intended to be used directly. Use one of the concrete
 * texture types such as @ref Texture2D, or derive your own texture class
 * that implements the required interface.
 *
 * @ingroup TexturesGroup
 */
class VGLX_EXPORT Texture : public Disposable {
public:
    /**
     * @brief Enumerates all supported texture types.
     */
    enum class Type {
        Texture2D, ///< Two-dimensional texture.
        DynamicTexture2D, ///< Two-dimensional dynamic GPU texture.
        CubeTexture, ///< Cube texture made up of six images.
    };

    /**
     * @brief Enumerates supported color spaces for texture sampling.
     *
     * Textures containing color data (e.g. albedo or base color maps) are
     * typically authored in sRGB and should use @ref ColorSpace "ColorSpace::sRGB".
     * Data textures (e.g. normals, roughness, metallic, masks) must use
     * @ref ColorSpace "ColorSpace::Linear" to preserve numerical correctness.
     */
    enum class ColorSpace {
        Linear, ///< Linear color space, no gamma correction is applied.
        sRGB ///< sRGB color space, converted to linear on sampling.
    };

    /**
     * @brief Enumerates pixel format and storage type for the texture.
     */
    enum class Format {
        RGBA8, ///< 8-bit normalized RGBA.
        RGBA16F, ///< 16-bit float RGBA.
        R16F, ///< 16-bit float single channel.
        R32F, ///< 32-bit float single channel.
        R32UI, ///< 32-bit unsigned integer single channel.
        SRGBA8 ///< 8-bit sRGB-encoded RGBA.
    };

    /**
     * @brief Specifies the byte alignment of each row in source texture data.
     *
     * Determines how pixel rows are aligned in memory when uploading texture
     * data to the GPU. The alignment value must match the source image’s
     * row stride to ensure correct decoding and avoid visual artifacts.
     *
     * RGBA8 textures typically use 4-byte row alignment.
     */
    enum class RowAlignment : uint8_t {
        OneByte = 1, ///< 1-byte alignment (tightly packed).
        TwoBytes = 2, ///< 2-byte alignment.
        FourBytes = 4, ///< 4-byte alignment (default).
        EightBytes = 8 ///< 8-byte alignment.
    };

    /**
     * @brief Enumerates supported minification filters.
     *
     * Minification filters control how a texture is sampled when it is rendered
     * smaller than its native resolution. Mipmapped filters select and blend
     * between mip levels when available.
     */
    enum class MinFilter {
        Nearest, ///< Nearest-neighbor sampling.
        Linear, ///< Linear interpolation.
        NearestMipmapNearest, ///< Nearest mip, nearest sample.
        LinearMipmapNearest, ///< Nearest mip, linear sample.
        NearestMipmapLinear, ///< Linear mip, nearest sample.
        LinearMipmapLinear ///< Linear mip, linear sample.
    };

    /**
     * @brief Enumerates supported magnification filters.
     *
     * Magnification filters control how a texture is sampled when it is rendered
     * larger than its native resolution.
     */
    enum class MagFilter {
        Nearest, ///< Nearest-neighbor sampling.
        Linear ///< Linear interpolation.
    };

    /**
     * @brief Enumerates supported texture wrapping modes.
     *
     * Wrapping modes control how a texture is sampled when texture coordinates
     * fall outside the $[0.0, 1.0]$ range, applied independently per axis.
     *
     * Cube maps ignore these modes and always clamp to edge to avoid visible
     * seams where faces meet.
     */
    enum class Wrapping {
        Repeat, ///< Tiles the texture, ignoring the integer part of the coordinate.
        ClampToEdge, ///< Clamps the coordinate to the edge texel.
        MirroredRepeat ///< Tiles the texture, mirroring on each repetition.
    };

    /**
     * @brief Enumerates supported texture mapping modes.
     *
     * Mapping describes how the texture's content should be interpreted when
     * sampled rather than how it is stored. A 2D texture marked as
     *
     * @ref Mapping "Mapping::Equirectangular" holds a full 360° panorama
     * (e.g. an HDR environment) and is sampled by direction instead of
     * texture coordinates.
     */
    enum class Mapping {
        UV, ///< Sampled with texture coordinates (default).
        Equirectangular ///< Panorama sampled by direction.
    };

    /// @brief Row alignment used when uploading texture data.
    RowAlignment row_alignment = RowAlignment::FourBytes;

    /// @brief Minification filter used when sampling the texture.
    MinFilter min_filter = MinFilter::Linear;

    /// @brief Magnification filter used when sampling the texture.
    MagFilter mag_filter = MagFilter::Linear;

    /// @brief Wrapping mode applied to the horizontal (U) texture coordinate.
    Wrapping wrap_s = Wrapping::Repeat;

    /// @brief Wrapping mode applied to the vertical (V) texture coordinate.
    Wrapping wrap_t = Wrapping::Repeat;

    /// @brief Mapping mode used to interpret the texture's content.
    Mapping mapping = Mapping::UV;

    /**
     * @brief Maximum anisotropy used when sampling this texture.
     *
     * Values greater than 1 enable anisotropic filtering, which improves
     * texture quality at oblique viewing angles. The value is clamped to
     * the driver-reported maximum at upload time. Only takes effect when
     * the texture is sampled with a mipmapped minification filter.
     * Defaults to 1 (isotropic).
     */
    float anisotropy {1.0f};

    /// @brief Enables automatic mipmap generation for this texture.
    bool generate_mipamps {false};

    /// @brief Texture storage format.
    Format format {Format::RGBA8};

    /// @brief Color space used when sampling this texture.
    ColorSpace color_space {ColorSpace::sRGB};

    /**
     * @brief Identifies the concrete @ref Texture::Type "texture type".
     */
    [[nodiscard]] virtual auto GetType() const -> Type = 0;
};

constexpr auto bytes_per_pixel(Texture::Format format) {
    using enum Texture::Format;
    switch (format) {
        case RGBA8:   return 4;
        case RGBA16F: return 8;
        case R16F:    return 2;
        case R32F:    return 4;
        case R32UI:   return 4;
        case SRGBA8:  return 4;
        default:      return 4;
    }
}

}
