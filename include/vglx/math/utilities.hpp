/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/math/constants.hpp"
#include "vglx/math/detail/tables.hpp"

#include <bit>
#include <cmath>
#include <cstdint>
#include <limits>

namespace vglx::math {

/**
 * @brief Clamps a value to the inclusive range [lo, hi].
 * @ingroup MathGroup
 *
 * @param v Input value.
 * @param lo Lower bound (inclusive).
 * @param hi Upper bound (inclusive).
 * @return Value clamped to [lo, hi].
 */
[[nodiscard]] constexpr auto Clamp(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

/**
 * @brief Converts degrees to radians.
 * @ingroup MathGroup
 *
 * @param degrees Angle in degrees.
 * @return Angle in radians.
 */
[[nodiscard]] constexpr auto DegToRad(const float degrees) {
    return degrees * pi / 180.0f;
}

/**
 * @brief Converts radians to degrees.
 * @ingroup MathGroup
 *
 * @param radians Angle in radians.
 * @return Angle in degrees.
 */
[[nodiscard]] constexpr auto RadToDeg(const float radians) {
    return radians * 180.0f / pi;
}

/**
 * @brief Linearly interpolates between two values.
 * @ingroup MathGroup
 *
 * @param a Start value.
 * @param b End value.
 * @param f Interpolation factor in [0, 1].
 * @return Interpolated value.
 */
[[nodiscard]] constexpr auto Lerp(float min, float max, const float x) {
    return std::lerp(min, max, x);
}

/**
 * @brief Smoothly interpolates between 0 and 1 with eased endpoints.
 * @ingroup MathGroup
 *
 * @param min Lower edge (must be less than max).
 * @param max Upper edge (must be greater than min).
 * @param x Input value.
 * @return Interpolated value.
 */
[[nodiscard]] constexpr auto Smoothstep(float min, float max, float x) {
    if (x <= min) return 0.0f;
    if (x >= max) return 1.0f;
    const auto t = (x - min) / (max - min);
    return t * t * (3.0f - 2.0f * t);
}

/**
 * @brief Returns the absolute value.
 * @ingroup MathGroup
 *
 * @param x Input float.
 * @return Absolute value.
 */
[[nodiscard]] constexpr auto Fabs(float x) {
    return (x < 0.0F) ? -x : x;
}
/**
 * @brief Approximates the exponential function e^x.
 *
 * Uses a combination of a precomputed integer-range lookup table
 * and a Taylor series polynomial correction for the fractional part.
 *
 * @param x Input exponent.
 */
[[nodiscard]] constexpr auto Exp(float x) -> float {
    if (x > 91.0f) return std::numeric_limits<float>::infinity();
    if (x < -88.0f) return 0.0f;

    const auto ix = static_cast<int32_t>(x);
    const auto f = static_cast<float>(ix - (x < static_cast<float>(ix)));
    const auto i = static_cast<int32_t>(f) + 88;
    if (i > 0) {
        x -= f;
        float r = 1.0f / 362880.0f;
        r = r * x + 1.0f / 40320.0f;
        r = r * x + 1.0f / 5040.0f;
        r = r * x + 1.0f / 720.0f;
        r = r * x + 1.0f / 120.0f;
        r = r * x + 1.0f / 24.0f;
        r = r * x + 1.0f / 6.0f;
        r = r * x + 0.5f;

        float poly = (r * (x * x) + (x + 1.0f));
        return poly * detail::GetExpValue(i < 179 ? i : 179);
    }
    return 0.0f;
}

/**
 * @brief Approximates the natural logarithm ln(x).
 *
 * Uses bit-manipulation to decompose the input into mantissa and exponent,
 * applying the identity ln(m * 2^e) = ln(m) + e * ln(2). The mantissa
 * is approximated using a Taylor series polynomial.
 *
 * @param x Input value (must be greater than 0).
 */
[[nodiscard]] constexpr auto Log(float x) -> float {
    if (x > 0) {
        auto i = std::bit_cast<int32_t>(x);
        auto e = (i >> 23) - 127;

        i = (i & 0x007FFFFF) | 0x3F800000;
        auto f = std::bit_cast<float>(i);

        if (f > 1.41421356f) {
            f *= 0.5f;
            e++;
        }

        auto m = f - 1.0f;
        auto r = 1.0f / 7.0f;
        r = r * m - 1.0f / 6.0f;
        r = r * m + 1.0f / 5.0f;
        r = r * m - 1.0f / 4.0f;
        r = r * m + 1.0f / 3.0f;
        r = r * m - 0.5f;

        return (r * (m * m) + m) + (static_cast<float>(e) * ln_2);
    }

    if (x == 0.0f) return -std::numeric_limits<float>::infinity();

    return std::numeric_limits<float>::quiet_NaN();
}

/**
 * @brief Computes base raised to the power of exponent (base^exponent).
 *
 * @param exponent The exponent value.
 */
[[nodiscard]] constexpr auto Pow(float base, float exponent) -> float {
    if (exponent == 0.0f) return 1.0f;
    if (base == 0.0f) return (exponent > 0.0f) ? 0.0f : std::numeric_limits<float>::infinity();
    if (base < 0.0f) return std::numeric_limits<float>::quiet_NaN();

    return Exp(Log(base) * exponent);
}

/**
 * @brief Computes square root using fast inverse sqrt refinement.
 * @ingroup MathGroup
 *
 * @param x Input value.
 * @return Approximated square root.
 */
[[nodiscard]] constexpr auto Sqrt(float x) {
    if (x <= 0.0f) {
        return 0.0f;
    }

    auto i = std::bit_cast<uint32_t>(x);
    i = 0x5F375A86 - (i >> 1);
    auto r = std::bit_cast<float>(i);
    r = (0.5f * r) * (3.0f - x * r * r);
    r = (0.5f * r) * (3.0f - x * r * r);

    return r * x;
}

/**
 * @brief Computes inverse square root using fast inverse sqrt refinement.
 * @ingroup MathGroup
 *
 * @param x Input value.
 * @return Approximated 1 / sqrt(x).
 */
[[nodiscard]] constexpr auto InverseSqrt(float x) {
    if (x <= 0.0f) {
        return std::numeric_limits<float>::infinity();
    }

    auto i = std::bit_cast<uint32_t>(x);
    i = 0x5F375A86 - (i >> 1);
    auto r = std::bit_cast<float>(i);
    r = (0.5f * r) * (3.0f - x * r * r);
    r = (0.5f * r) * (3.0f - x * r * r);

    return r;
}

/**
 * @brief Approximates cosine using lookup + polynomial correction.
 * @ingroup MathGroup
 *
 * @param x Angle in radians.
 * @return Approximate cosine.
 */
[[nodiscard]] constexpr auto Cos(float x) {
    auto b = Fabs(x) * inv_tau;
    auto i = static_cast<int32_t>(b);
    b = (b - float(i)) * tau_over_256;

    auto cossin_alpha = detail::GetTrigPair(i & 255);

    auto b2 = b * b;
    auto sine_beta = b - b * b2 * (0.1666666667f - b2 * 0.0083333333f);
    auto cosine_beta = 1.0f - b2 * (0.5f - b2 * 0.0416666667f);

    return cossin_alpha.x * cosine_beta - cossin_alpha.y * sine_beta;
}

/**
 * @brief Approximates sine using lookup + polynomial correction.
 * @ingroup MathGroup
 *
 * @param x Angle in radians.
 * @return Approximate sine.
 */
[[nodiscard]] constexpr auto Sin(float x) {
    auto b = Fabs(x) * inv_tau;
    auto i = static_cast<int32_t>(b);
    b = (b - float(i)) * tau_over_256;

    auto cossin_alpha = detail::GetTrigPair(i & 255);

    auto b2 = b * b;
    auto sine_beta = b - b * b2 * (0.1666666667f - b2 * 0.0083333333f);
    auto cosine_beta = 1.0f - b2 * (0.5f - b2 * 0.0416666667f);

    auto sine = cossin_alpha.y * cosine_beta + cossin_alpha.x * sine_beta;
    return x < 0.0f ? -sine : sine;
}

/**
 * @brief Approximates arctangent of a single value.
 * @ingroup MathGroup
 *
 * Uses piecewise approximation with a small lookup table.
 *
 * @param x Input value.
 * @return atan(x) in radians.
 */
[[nodiscard]] constexpr auto Atan(float x) {
    auto a = Fabs(x);

    if (a <= 1.0f) {
        auto b = a * 64.0f;
        auto i = static_cast<int32_t>(b);
        b = float(i) * 0.015625f; // inv_table_size: 1.0f / 64.0f

        auto arctan_b = detail::GetArctanValue(i);
        auto c = (a - b) / (a * b + 1.0F);
        auto c2 = c * c;

        auto arctan_c = c * (1.0F - c2 * (0.3333333333f + c2 * (0.2f - c2 * 0.1428571429f)));
        a = arctan_b + arctan_c;
    } else {
        a = 1.0f / a;
        auto b = a * 64.0f;
        auto i = static_cast<int32_t>(b);
        b = float(i) * 0.015625f;

        auto arctan_b = detail::GetArctanValue(i);
        auto c = (a - b) / (a * b + 1.0F);
        auto c2 = c * c;

        auto arctan_c = c * (1.0F - c2 * (0.3333333333f + c2 * (0.2f - c2 * 0.1428571429f)));
        a = tau_over_4 - (arctan_b + arctan_c);
    }

    return x < 0.0f ? -a : a;
}

/**
 * @brief Approximates atan2 with quadrant correction.
 * @ingroup MathGroup
 *
 * @param y Y-coordinate.
 * @param x X-coordinate.
 * @return Angle in radians from positive X-axis.
 */
[[nodiscard]] constexpr auto Atan2(float y, float x) {
    if (Fabs(x) > eps) {
        auto r = Atan(y / x);
        if (x < 0.0f) {
            return y >= 0.0f ? r + math::pi : r - math::pi;
        }
        return r;
    }

    if (Fabs(y) > eps) {
        return y > 0.0f ? math::pi_over_2 : -math::pi_over_2;
    }

    return 0.0f;
}

/**
 * @brief Approximates arcsine using arctangent and inverse sqrt.
 * @ingroup MathGroup
 *
 * @param y Sine value in [-1, 1].
 * @return Angle in radians.
 */
[[nodiscard]] constexpr auto Asin(float y) {
    return (Atan(y * InverseSqrt(1.0f - y * y)));
}

/**
 * @brief Converts an sRGB channel value to linear.
 *
 * @param c Input and output are in the range [0, 1].
 */
[[nodiscard]] constexpr auto SRGBToLinear(float c) -> float {
    c = Clamp(c, 0.0f, 1.0f);
    if (c <= 0.04045f) {
        return c / 12.92f;
    }
    return Pow((c + 0.055f) / 1.055f, 2.4f);
}

/**
 * @brief Converts a linear channel value to sRGB.
 *
 * @param c Input and output are in the range [0, 1].
 */
[[nodiscard]] constexpr auto LinearToSRGB(float c) -> float {
    c = Clamp(c, 0.0f, 1.0f);
    if (c <= 0.0031308f) {
        return c * 12.92f;
    }
    return 1.055f * Pow(c, 1.0f / 2.4f) - 0.055f;
}

}
