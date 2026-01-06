/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include <bit>
#include <cinttypes>
#include <cmath>
#include <limits>
#include <random>
#include <sstream>
#include <string>

namespace vglx::math {

using Pair = struct { float x, y; };

constexpr float pi = 3.1415926535897932384626433832795f;
constexpr float two_pi = 6.2831853071795864769252867665590f;
constexpr float pi_over_2 = 1.5707963267948966192313216916398f;
constexpr float pi_over_4 = 0.78539816339744830961566084581988f;
constexpr float pi_over_6 = 0.52359877559829887307710723054658f;
constexpr float tau = 6.2831853071795864769252867665590f;
constexpr float tau_over_2 = 3.1415926535897932384626433832795f;
constexpr float tau_over_4 = 1.5707963267948966192313216916398f;
constexpr float tau_over_6 = 1.0471975511965977461542144610932f;
constexpr float tau_over_256 = 0.0245436926f;
constexpr float inv_tau = 40.74366543f;
constexpr float eps = 1e-6f;

alignas(64) constexpr uint32_t trig_table[256][2] = {
	{0x3F800000, 0x00000000}, {0x3F7FEC43, 0x3CC90AB0}, {0x3F7FB10F, 0x3D48FB30}, {0x3F7F4E6D, 0x3D96A905}, {0x3F7EC46D, 0x3DC8BD36}, {0x3F7E1324, 0x3DFAB273}, {0x3F7D3AAC, 0x3E164083}, {0x3F7C3B28, 0x3E2F10A3},
	{0x3F7B14BE, 0x3E47C5C2}, {0x3F79C79D, 0x3E605C13}, {0x3F7853F8, 0x3E78CFCD}, {0x3F76BA07, 0x3E888E94}, {0x3F74FA0B, 0x3E94A031}, {0x3F731447, 0x3EA09AE5}, {0x3F710908, 0x3EAC7CD4}, {0x3F6ED89E, 0x3EB8442A},
	{0x3F6C835E, 0x3EC3EF16}, {0x3F6A09A6, 0x3ECF7BCB}, {0x3F676BD8, 0x3EDAE880}, {0x3F64AA59, 0x3EE63375}, {0x3F61C597, 0x3EF15AEA}, {0x3F5EBE05, 0x3EFC5D28}, {0x3F5B941A, 0x3F039C3D}, {0x3F584853, 0x3F08F59B},
	{0x3F54DB31, 0x3F0E39DA}, {0x3F514D3D, 0x3F13682B}, {0x3F4D9F02, 0x3F187FC0}, {0x3F49D112, 0x3F1D7FD2}, {0x3F45E403, 0x3F22679A}, {0x3F41D870, 0x3F273656}, {0x3F3DAEF9, 0x3F2BEB4A}, {0x3F396842, 0x3F3085BB},
	{0x3F3504F3, 0x3F3504F3}, {0x3F3085BA, 0x3F396842}, {0x3F2BEB49, 0x3F3DAEFA}, {0x3F273655, 0x3F41D871}, {0x3F226799, 0x3F45E403}, {0x3F1D7FD1, 0x3F49D112}, {0x3F187FC0, 0x3F4D9F02}, {0x3F13682A, 0x3F514D3D},
	{0x3F0E39D9, 0x3F54DB32}, {0x3F08F59B, 0x3F584853}, {0x3F039C3C, 0x3F5B941B}, {0x3EFC5D27, 0x3F5EBE05}, {0x3EF15AE7, 0x3F61C598}, {0x3EE63374, 0x3F64AA59}, {0x3EDAE881, 0x3F676BD8}, {0x3ECF7BC9, 0x3F6A09A7},
	{0x3EC3EF15, 0x3F6C835E}, {0x3EB84427, 0x3F6ED89E}, {0x3EAC7CD3, 0x3F710908}, {0x3EA09AE2, 0x3F731448}, {0x3E94A030, 0x3F74FA0B}, {0x3E888E93, 0x3F76BA07}, {0x3E78CFC8, 0x3F7853F8}, {0x3E605C12, 0x3F79C79D},
	{0x3E47C5BC, 0x3F7B14BF}, {0x3E2F10A0, 0x3F7C3B28}, {0x3E164085, 0x3F7D3AAC}, {0x3DFAB26C, 0x3F7E1324}, {0x3DC8BD35, 0x3F7EC46D}, {0x3D96A8FB, 0x3F7F4E6D}, {0x3D48FB29, 0x3F7FB10F}, {0x3CC90A7E, 0x3F7FEC43},
	{0x00000000, 0x3F800000}, {0xBCC90A7E, 0x3F7FEC43}, {0xBD48FB29, 0x3F7FB10F}, {0xBD96A8FB, 0x3F7F4E6D}, {0xBDC8BD35, 0x3F7EC46D}, {0xBDFAB26C, 0x3F7E1324}, {0xBE164085, 0x3F7D3AAC}, {0xBE2F10A0, 0x3F7C3B28},
	{0xBE47C5BC, 0x3F7B14BF}, {0xBE605C12, 0x3F79C79D}, {0xBE78CFC8, 0x3F7853F8}, {0xBE888E93, 0x3F76BA07}, {0xBE94A030, 0x3F74FA0B}, {0xBEA09AE2, 0x3F731448}, {0xBEAC7CD3, 0x3F710908}, {0xBEB84427, 0x3F6ED89E},
	{0xBEC3EF15, 0x3F6C835E}, {0xBECF7BC9, 0x3F6A09A7}, {0xBEDAE881, 0x3F676BD8}, {0xBEE63374, 0x3F64AA59}, {0xBEF15AE7, 0x3F61C598}, {0xBEFC5D27, 0x3F5EBE05}, {0xBF039C3C, 0x3F5B941B}, {0xBF08F59B, 0x3F584853},
	{0xBF0E39D9, 0x3F54DB32}, {0xBF13682A, 0x3F514D3D}, {0xBF187FC0, 0x3F4D9F02}, {0xBF1D7FD1, 0x3F49D112}, {0xBF226799, 0x3F45E403}, {0xBF273655, 0x3F41D871}, {0xBF2BEB49, 0x3F3DAEFA}, {0xBF3085BA, 0x3F396842},
	{0xBF3504F3, 0x3F3504F3}, {0xBF396842, 0x3F3085BB}, {0xBF3DAEF9, 0x3F2BEB4A}, {0xBF41D870, 0x3F273656}, {0xBF45E403, 0x3F22679A}, {0xBF49D112, 0x3F1D7FD2}, {0xBF4D9F02, 0x3F187FC0}, {0xBF514D3D, 0x3F13682B},
	{0xBF54DB31, 0x3F0E39DA}, {0xBF584853, 0x3F08F59B}, {0xBF5B941A, 0x3F039C3D}, {0xBF5EBE05, 0x3EFC5D28}, {0xBF61C597, 0x3EF15AEA}, {0xBF64AA59, 0x3EE63375}, {0xBF676BD8, 0x3EDAE880}, {0xBF6A09A6, 0x3ECF7BCB},
	{0xBF6C835E, 0x3EC3EF16}, {0xBF6ED89E, 0x3EB8442A}, {0xBF710908, 0x3EAC7CD4}, {0xBF731447, 0x3EA09AE5}, {0xBF74FA0B, 0x3E94A031}, {0xBF76BA07, 0x3E888E94}, {0xBF7853F8, 0x3E78CFCD}, {0xBF79C79D, 0x3E605C13},
	{0xBF7B14BE, 0x3E47C5C2}, {0xBF7C3B28, 0x3E2F10A3}, {0xBF7D3AAC, 0x3E164083}, {0xBF7E1324, 0x3DFAB273}, {0xBF7EC46D, 0x3DC8BD36}, {0xBF7F4E6D, 0x3D96A905}, {0xBF7FB10F, 0x3D48FB30}, {0xBF7FEC43, 0x3CC90AB0},
	{0xBF800000, 0x00000000}, {0xBF7FEC43, 0xBCC90AB0}, {0xBF7FB10F, 0xBD48FB30}, {0xBF7F4E6D, 0xBD96A905}, {0xBF7EC46D, 0xBDC8BD36}, {0xBF7E1324, 0xBDFAB273}, {0xBF7D3AAC, 0xBE164083}, {0xBF7C3B28, 0xBE2F10A3},
	{0xBF7B14BE, 0xBE47C5C2}, {0xBF79C79D, 0xBE605C13}, {0xBF7853F8, 0xBE78CFCD}, {0xBF76BA07, 0xBE888E94}, {0xBF74FA0B, 0xBE94A031}, {0xBF731447, 0xBEA09AE5}, {0xBF710908, 0xBEAC7CD4}, {0xBF6ED89E, 0xBEB8442A},
	{0xBF6C835E, 0xBEC3EF16}, {0xBF6A09A6, 0xBECF7BCB}, {0xBF676BD8, 0xBEDAE880}, {0xBF64AA59, 0xBEE63375}, {0xBF61C597, 0xBEF15AEA}, {0xBF5EBE05, 0xBEFC5D28}, {0xBF5B941A, 0xBF039C3D}, {0xBF584853, 0xBF08F59B},
	{0xBF54DB31, 0xBF0E39DA}, {0xBF514D3D, 0xBF13682B}, {0xBF4D9F02, 0xBF187FC0}, {0xBF49D112, 0xBF1D7FD2}, {0xBF45E403, 0xBF22679A}, {0xBF41D870, 0xBF273656}, {0xBF3DAEF9, 0xBF2BEB4A}, {0xBF396842, 0xBF3085BB},
	{0xBF3504F3, 0xBF3504F3}, {0xBF3085BA, 0xBF396842}, {0xBF2BEB49, 0xBF3DAEFA}, {0xBF273655, 0xBF41D871}, {0xBF226799, 0xBF45E403}, {0xBF1D7FD1, 0xBF49D112}, {0xBF187FC0, 0xBF4D9F02}, {0xBF13682A, 0xBF514D3D},
	{0xBF0E39D9, 0xBF54DB32}, {0xBF08F59B, 0xBF584853}, {0xBF039C3C, 0xBF5B941B}, {0xBEFC5D27, 0xBF5EBE05}, {0xBEF15AE7, 0xBF61C598}, {0xBEE63374, 0xBF64AA59}, {0xBEDAE881, 0xBF676BD8}, {0xBECF7BC9, 0xBF6A09A7},
	{0xBEC3EF15, 0xBF6C835E}, {0xBEB84427, 0xBF6ED89E}, {0xBEAC7CD3, 0xBF710908}, {0xBEA09AE2, 0xBF731448}, {0xBE94A030, 0xBF74FA0B}, {0xBE888E93, 0xBF76BA07}, {0xBE78CFC8, 0xBF7853F8}, {0xBE605C12, 0xBF79C79D},
	{0xBE47C5BC, 0xBF7B14BF}, {0xBE2F10A0, 0xBF7C3B28}, {0xBE164085, 0xBF7D3AAC}, {0xBDFAB26C, 0xBF7E1324}, {0xBDC8BD35, 0xBF7EC46D}, {0xBD96A8FB, 0xBF7F4E6D}, {0xBD48FB29, 0xBF7FB10F}, {0xBCC90A7E, 0xBF7FEC43},
	{0x00000000, 0xBF800000}, {0x3CC90A7E, 0xBF7FEC43}, {0x3D48FB29, 0xBF7FB10F}, {0x3D96A8FB, 0xBF7F4E6D}, {0x3DC8BD35, 0xBF7EC46D}, {0x3DFAB26C, 0xBF7E1324}, {0x3E164085, 0xBF7D3AAC}, {0x3E2F10A0, 0xBF7C3B28},
	{0x3E47C5BC, 0xBF7B14BF}, {0x3E605C12, 0xBF79C79D}, {0x3E78CFC8, 0xBF7853F8}, {0x3E888E93, 0xBF76BA07}, {0x3E94A030, 0xBF74FA0B}, {0x3EA09AE2, 0xBF731448}, {0x3EAC7CD3, 0xBF710908}, {0x3EB84427, 0xBF6ED89E},
	{0x3EC3EF15, 0xBF6C835E}, {0x3ECF7BC9, 0xBF6A09A7}, {0x3EDAE881, 0xBF676BD8}, {0x3EE63374, 0xBF64AA59}, {0x3EF15AE7, 0xBF61C598}, {0x3EFC5D27, 0xBF5EBE05}, {0x3F039C3C, 0xBF5B941B}, {0x3F08F59B, 0xBF584853},
	{0x3F0E39D9, 0xBF54DB32}, {0x3F13682A, 0xBF514D3D}, {0x3F187FC0, 0xBF4D9F02}, {0x3F1D7FD1, 0xBF49D112}, {0x3F226799, 0xBF45E403}, {0x3F273655, 0xBF41D871}, {0x3F2BEB49, 0xBF3DAEFA}, {0x3F3085BA, 0xBF396842},
	{0x3F3504F3, 0xBF3504F3}, {0x3F396842, 0xBF3085BB}, {0x3F3DAEF9, 0xBF2BEB4A}, {0x3F41D870, 0xBF273656}, {0x3F45E403, 0xBF22679A}, {0x3F49D112, 0xBF1D7FD2}, {0x3F4D9F02, 0xBF187FC0}, {0x3F514D3D, 0xBF13682B},
	{0x3F54DB31, 0xBF0E39DA}, {0x3F584853, 0xBF08F59B}, {0x3F5B941A, 0xBF039C3D}, {0x3F5EBE05, 0xBEFC5D28}, {0x3F61C597, 0xBEF15AEA}, {0x3F64AA59, 0xBEE63375}, {0x3F676BD8, 0xBEDAE880}, {0x3F6A09A6, 0xBECF7BCB},
	{0x3F6C835E, 0xBEC3EF16}, {0x3F6ED89E, 0xBEB8442A}, {0x3F710908, 0xBEAC7CD4}, {0x3F731447, 0xBEA09AE5}, {0x3F74FA0B, 0xBE94A031}, {0x3F76BA07, 0xBE888E94}, {0x3F7853F8, 0xBE78CFCD}, {0x3F79C79D, 0xBE605C13},
	{0x3F7B14BE, 0xBE47C5C2}, {0x3F7C3B28, 0xBE2F10A3}, {0x3F7D3AAC, 0xBE164083}, {0x3F7E1324, 0xBDFAB273}, {0x3F7EC46D, 0xBDC8BD36}, {0x3F7F4E6D, 0xBD96A905}, {0x3F7FB10F, 0xBD48FB30}, {0x3F7FEC43, 0xBCC90AB0}
};

alignas(64) constexpr uint32_t arctan_table[65] {
    0x00000000, 0x3C7FFAAB, 0x3CFFEAAE, 0x3D3FDC0C, 0x3D7FAADE, 0x3D9FACF8, 0x3DBF70C1, 0x3DDF1CF6,
    0x3DFEADD5, 0x3E0F0FD8, 0x3E1EB777, 0x3E2E4C09, 0x3E3DCBDA, 0x3E4D3547, 0x3E5C86BB, 0x3E6BBEAF,
    0x3E7ADBB0, 0x3E84EE2D, 0x3E8C5FAD, 0x3E93C1B9, 0x3E9B13BA, 0x3EA25522, 0x3EA9856D, 0x3EB0A420,
    0x3EB7B0CA, 0x3EBEAB02, 0x3EC5926A, 0x3ECC66AA, 0x3ED32776, 0x3ED9D489, 0x3EE06DA6, 0x3EE6F29A,
    0x3EED6338, 0x3EF3BF5C, 0x3EFA06E8, 0x3F001CE4, 0x3F032BF5, 0x3F0630A3, 0x3F092AED, 0x3F0C1AD4,
    0x3F0F005D, 0x3F11DB8F, 0x3F14AC73, 0x3F177314, 0x3F1A2F81, 0x3F1CE1C9, 0x3F1F89FE, 0x3F222833,
    0x3F24BC7D, 0x3F2746F3, 0x3F29C7AC, 0x3F2C3EC1, 0x3F2EAC4C, 0x3F311069, 0x3F336B32, 0x3F35BCC5,
    0x3F38053E, 0x3F3A44BC, 0x3F3C7B5E, 0x3F3EA941, 0x3F40CE86, 0x3F42EB4B, 0x3F44FFB0, 0x3F470BD5, 0x3F490FDB
};

[[nodiscard]] constexpr Pair GetTrigPair(int32_t index) {
    return std::bit_cast<Pair>(trig_table[index & 255]);
}

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
[[nodiscard]] constexpr auto Lerp(const float a, const float b, const float f) {
    return std::lerp(a, b, f);
}

/**
 * @brief Computes the Cantor pairing of two values.
 * @ingroup MathGroup
 *
 * @tparam T Integer-like type.
 * @param x First value.
 * @param y Second value.
 * @return Unique paired value.
 */
[[nodiscard]] constexpr auto CantorPairing(const auto x, const auto y) {
    return ((x + y) * (x + y + 1)) / 2 + y;
}

/**
 * @brief Computes unordered Cantor pairing (order-invariant).
 * @ingroup MathGroup
 *
 * @tparam T Integer-like type.
 * @param x First value.
 * @param y Second value.
 * @return Unique paired value, independent of input order.
 */
[[nodiscard]] constexpr auto CantorPairingUnordered(const auto x, const auto y) {
    return x > y ? CantorPairing(y, x) : CantorPairing(x, y);
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

    auto cossin_alpha = GetTrigPair(i & 255);

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

    auto cossin_alpha = GetTrigPair(i & 255);

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

        auto arctan_b = std::bit_cast<float>(arctan_table[i]);
        auto c = (a - b) / (a * b + 1.0F);
        auto c2 = c * c;

        auto arctan_c = c * (1.0F - c2 * (0.3333333333f + c2 * (0.2f - c2 * 0.1428571429f)));
        a = arctan_b + arctan_c;
    } else {
        a = 1.0f / a;
        auto b = a * 64.0f;
        auto i = static_cast<int32_t>(b);
        b = float(i) * 0.015625f;

        auto arctan_b = std::bit_cast<float>(arctan_table[i]);
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
    return std::pow((c + 0.055f) / 1.055f, 2.4f);
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
    return 1.055f * std::pow(c, 1.0f / 2.4f) - 0.055f;
}

/**
 * @brief Generates a UUID string (version 4-like).
 * @ingroup MathGroup
 *
 * @return Random UUID as a string.
 */
[[nodiscard]] VGLX_EXPORT inline auto GenerateUUID() {
    static std::vector<std::string> lut{
        "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0a",
        "0b", "0c", "0d", "0e", "0f", "10", "11", "12", "13", "14", "15",
        "16", "17", "18", "19", "1a", "1b", "1c", "1d", "1e", "1f", "20",
        "21", "22", "23", "24", "25", "26", "27", "28", "29", "2a", "2b",
        "2c", "2d", "2e", "2f", "30", "31", "32", "33", "34", "35", "36",
        "37", "38", "39", "3a", "3b", "3c", "3d", "3e", "3f", "40", "41",
        "42", "43", "44", "45", "46", "47", "48", "49", "4a", "4b", "4c",
        "4d", "4e", "4f", "50", "51", "52", "53", "54", "55", "56", "57",
        "58", "59", "5a", "5b", "5c", "5d", "5e", "5f", "60", "61", "62",
        "63", "64", "65", "66", "67", "68", "69", "6a", "6b", "6c", "6d",
        "6e", "6f", "70", "71", "72", "73", "74", "75", "76", "77", "78",
        "79", "7a", "7b", "7c", "7d", "7e", "7f", "80", "81", "82", "83",
        "84", "85", "86", "87", "88", "89", "8a", "8b", "8c", "8d", "8e",
        "8f", "90", "91", "92", "93", "94", "95", "96", "97", "98", "99",
        "9a", "9b", "9c", "9d", "9e", "9f", "a0", "a1", "a2", "a3", "a4",
        "a5", "a6", "a7", "a8", "a9", "aa", "ab", "ac", "ad", "ae", "af",
        "b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7", "b8", "b9", "ba",
        "bb", "bc", "bd", "be", "bf", "c0", "c1", "c2", "c3", "c4", "c5",
        "c6", "c7", "c8", "c9", "ca", "cb", "cc", "cd", "ce", "cf", "d0",
        "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9", "da", "db",
        "dc", "dd", "de", "df", "e0", "e1", "e2", "e3", "e4", "e5", "e6",
        "e7", "e8", "e9", "ea", "eb", "ec", "ed", "ee", "ef", "f0", "f1",
        "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "fa", "fb", "fc",
        "fd", "fe", "ff"
    };

    static std::random_device rd;
    static std::mt19937 e2(rd());
    static std::uniform_real_distribution dist(0.0, 1.0);

    const auto d0 = static_cast<size_t>(dist(e2) * 0xffffffff) | 0;
    const auto d1 = static_cast<size_t>(dist(e2) * 0xffffffff) | 0;
    const auto d2 = static_cast<size_t>(dist(e2) * 0xffffffff) | 0;
    const auto d3 = static_cast<size_t>(dist(e2) * 0xffffffff) | 0;

    auto uuid = lut[d0 & 0xff] + lut[d0 >> 8 & 0xff] + lut[d0 >> 16 & 0xff] +
                lut[d0 >> 24 & 0xff] + '-' + lut[d1 & 0xff] +
                lut[d1 >> 8 & 0xff]  + '-' + lut[d1 >> 16 & 0x0f | 0x40] +
                lut[d1 >> 24 & 0xff] + '-' + lut[d2 & 0x3f | 0x80] +
                lut[d2 >> 8 & 0xff]  + '-' + lut[d2 >> 16 & 0xff] +
                lut[d2 >> 24 & 0xff] + lut[d3 & 0xff] + lut[d3 >> 8 & 0xff] +
                lut[d3 >> 16 & 0xff] + lut[d3 >> 24 & 0xff];

    return uuid;
}

}