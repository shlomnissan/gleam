/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/math/utilities.hpp"

#include <algorithm>
#include <cassert>
#include <span>

namespace vglx {

/**
 * @brief Represents an RGB color with floating-point components.
 *
 * This class stores color values as three floating-point channels (red, green,
 * blue), normalized to the range $[0.0, 1.0]$. It does not include an alpha
 * component. Color supports construction from floats, hexadecimal values, and
 * spans, as well as basic arithmetic operations.
 *
 * @ingroup MathGroup
 */
class Color  {
public:
    float r {1.0f}; ///< Red channel in $[0.0, 1.0]$.
    float g {1.0f}; ///< Green channel in $[0.0, 1.0]$.
    float b {1.0f}; ///< Blue channel in $[0.0, 1.0]$.

    /**
     * @brief Constructs a color with default RGB values.
     */
    constexpr Color() = default;

    /**
     * @brief Constructs a color from individual RGB components.
     *
     * The components are assumed to be in linear color space.
     *
     * @param r Red component.
     * @param g Green component.
     * @param b Blue component.
     */
    constexpr Color(float r, float g, float b) : r(r), g(g), b(b) {}

    /**
     * @brief Constructs a color from a hexadecimal value.
     *
     * The format is `0xRRGGBB`. The hexadecimal value is interpreted as an sRGB
     * color (as commonly used in CSS and color pickers) and is converted to linear
     * RGB for storage in this Color instance.
     *
     * @param hex Hexadecimal color code in `0xRRGGBB` format (sRGB).
     */
    constexpr Color(unsigned int hex) {
        const float sr = static_cast<float>((hex >> 16) & 255) / 255.f;
        const float sg = static_cast<float>((hex >> 8)  & 255) / 255.f;
        const float sb = static_cast<float>((hex) & 255) / 255.f;

        r = math::SRGBToLinear(sr);
        g = math::SRGBToLinear(sg);
        b = math::SRGBToLinear(sb);
    }

    /**
     * @brief Constructs a color from a span of three float values.
     *
     * @param color Span containing red, green, and blue components.
     */
    explicit constexpr Color(std::span<float> color) : r(color[0]), g(color[1]), b(color[2]) {}

    /**
     * @brief Accesses a channel by index.
     *
     * @param i Index: `0 → r`, `1 → g`, `2 → b`.
     */
    [[nodiscard]] constexpr auto operator[](int i) -> float& {
        assert(i >= 0 && i < 3);
        switch (i) {
            case 0: return r;
            case 1: return g;
            case 2: return b;
            default: return r; // unreachable
        }
    }

    /**
     * @brief Accesses a channel by index.
     *
     * @param i Index: `0 → r`, `1 → g`, `2 → b`.
     */
    [[nodiscard]] constexpr auto operator[](int i) const -> float {
        assert(i >= 0 && i < 3);
        switch (i) {
            case 0: return r;
            case 1: return g;
            case 2: return b;
            default: return r; // unreachable
        }
    }

    /**
     * @brief Assigns a new color from a hexadecimal value.
     *
     * @param hex Hexadecimal color code in `0xRRGGBB` format.
     */
    constexpr auto operator=(unsigned int hex) -> Color& {
        r = static_cast<float>(hex >> 16 & 255) / 255.f;
        g = static_cast<float>(hex >> 8 & 255) / 255.f;
        b = static_cast<float>(hex & 255) / 255.f;
        return *this;
    }

    /**
     * @brief Multiplies the color by a scalar in-place.
     *
     * @param n Scalar value.
     */
    constexpr auto operator*=(float n) -> Color& {
        r = r * n;
        g = g * n;
        b = b * n;
        return *this;
    }

    /**
     * @brief Compares two Color objects for equality.
     */
    constexpr auto operator==(const Color&) const -> bool = default;
};

/**
 * @brief Adds two colors component-wise.
 * @related Color
 *
 * @param a First color.
 * @param b Second color.
 */
[[nodiscard]] constexpr auto operator+(const Color& a, const Color& b) -> Color {
    return Color {a.r + b.r, a.g + b.g, a.b + b.b};
}

/**
 * @brief Subtracts one color from another, clamped at zero.
 * @related Color
 *
 * @param a First color.
 * @param b Color to subtract.
 */
[[nodiscard]] constexpr auto operator-(const Color& a, const Color& b) -> Color {
    return Color {
        std::max(0.0f, a.r - b.r),
        std::max(0.0f, a.g - b.g),
        std::max(0.0f, a.b - b.b)
    };
}

/**
 * @brief Multiplies a color by a scalar.
 * @related Color
 *
 * @param v Input color.
 * @param n Scalar value.
 */
[[nodiscard]] constexpr auto operator*(const Color& v, float n) -> Color {
    return Color {v.r * n, v.g * n, v.b * n};
}

/**
 * @brief Multiplies a scalar by a color.
 * @related Color
 *
 * @param n Scalar value.
 * @param v Input color.
 */
[[nodiscard]] constexpr auto operator*(float n, const Color& v) -> Color {
    return v * n;
}

/**
 * @brief Linearly interpolates between two colors.
 * @related Color
 *
 * @param a Start color.
 * @param b End color.
 * @param f Interpolation factor in $[0, 1]$.
 */
[[nodiscard]] constexpr auto Lerp(const Color& a, const Color& b, float f) -> Color {
    return Color {
        math::Lerp(a.r, b.r, f),
        math::Lerp(a.g, b.g, f),
        math::Lerp(a.b, b.b, f)
    };
}

}