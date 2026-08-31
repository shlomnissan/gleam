/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>

#include "test_helpers.hpp"

#include <cassert>

#include <vglx/math/utilities.hpp>

using namespace vglx;

#pragma region Degrees and Radians

TEST(MathUtilities, DegToRad) {
    static_assert(math::DegToRad(0.0f) == 0.0f);
    static_assert(math::DegToRad(90.0f) == math::pi / 2.0f);
    static_assert(math::DegToRad(180.0f) == math::pi);
    static_assert(math::DegToRad(360.0f) == math::two_pi);
    static_assert(math::DegToRad(45.0f) == math::pi / 4.0f);

    EXPECT_FLOAT_EQ(math::DegToRad(0.0f), 0.0f);
    EXPECT_FLOAT_EQ(math::DegToRad(90.0f), math::pi / 2.0f);
    EXPECT_FLOAT_EQ(math::DegToRad(180.0f), math::pi);
    EXPECT_FLOAT_EQ(math::DegToRad(360.0f), math::two_pi);
    EXPECT_FLOAT_EQ(math::DegToRad(45.0f), math::pi / 4.0f);
}

TEST(MathUtilities, RadToDeg) {
    static_assert(math::RadToDeg(0.0f) == 0.0f);
    static_assert(math::RadToDeg(math::pi / 2.0f) == 90.0f);
    static_assert(math::RadToDeg(math::pi) == 180.0f);
    static_assert(math::RadToDeg(math::two_pi) == 360.0f);
    static_assert(math::RadToDeg(math::pi / 4.0f) == 45.0f);

    EXPECT_FLOAT_EQ(math::RadToDeg(0.0f), 0.0f);
    EXPECT_FLOAT_EQ(math::RadToDeg(math::pi / 2.0f), 90.0f);
    EXPECT_FLOAT_EQ(math::RadToDeg(math::pi), 180.0f);
    EXPECT_FLOAT_EQ(math::RadToDeg(math::two_pi), 360.0f);
    EXPECT_FLOAT_EQ(math::RadToDeg(math::pi / 4.0f), 45.0f);
}

#pragma endregion

#pragma region Sqrt

TEST(MathUtilities, SqrtAccuracyCommonCases) {
    EXPECT_NEAR(math::Sqrt(0.0f), 0.0f, 1e-4f);
    EXPECT_NEAR(math::Sqrt(1.0f), 1.0f, 1e-4f);
    EXPECT_NEAR(math::Sqrt(2.0f), 1.4142f, 1e-4f);
    EXPECT_NEAR(math::Sqrt(4.0f), 2.0f, 1e-4f);
    EXPECT_NEAR(math::Sqrt(9.0f), 3.0f, 1e-4f);
    EXPECT_NEAR(math::Sqrt(16.0f), 4.0f, 1e-4f);
    EXPECT_NEAR(math::Sqrt(100.0f), 10.0f, 1e-3f);

    static_assert(ApproxEqual(math::Sqrt(1.0f), 1.0f));
    static_assert(ApproxEqual(math::Sqrt(0.0f), 0.0f));
    static_assert(math::Sqrt(4.0f) > 1.9f && math::Sqrt(4.0f) < 2.1f);
    static_assert(math::Sqrt(9.0f) > 2.9f && math::Sqrt(9.0f) < 3.1f);
}

TEST(MathUtilities, SqrtTinyValues) {
    EXPECT_NEAR(math::Sqrt(1e-10f), 0.0f, 1e-4f);
    EXPECT_NEAR(math::Sqrt(std::numeric_limits<float>::min()), 0.0f, 1e-4f);

    static_assert(ApproxEqual(math::Sqrt(1e-10f), 0.0f));
    static_assert(ApproxEqual(math::Sqrt(std::numeric_limits<float>::min()), 0.0f));
}

TEST(MathUtilities, SqrtNegativeInputBehavior) {
    EXPECT_EQ(math::Sqrt(-1.0f), 0.0f);

    static_assert(math::Sqrt(-1.0f) == 0.0f);
}

TEST(MathUtilities, InverseSqrtAccuracyCommonCases) {
    EXPECT_NEAR(math::InverseSqrt(1.0f), 1.0f, 1e-4f);
    EXPECT_NEAR(math::InverseSqrt(4.0f), 0.5f, 1e-4f);
    EXPECT_NEAR(math::InverseSqrt(9.0f), 1.0f / 3.0f, 1e-4f);
    EXPECT_NEAR(math::InverseSqrt(16.0f), 0.25f, 1e-4f);
    EXPECT_NEAR(math::InverseSqrt(100.0f), 0.1f, 1e-3f);

    static_assert(ApproxEqual(math::InverseSqrt(1.0f), 1.0f));
    static_assert(math::InverseSqrt(4.0f) > 0.49f && math::InverseSqrt(4.0f) < 0.51f);
    static_assert(math::InverseSqrt(9.0f) > 0.32f && math::InverseSqrt(9.0f) < 0.35f);
}

TEST(MathUtilities, InverseSqrtTinyValues) {
    EXPECT_TRUE(std::isinf(math::InverseSqrt(0.0f)));
    EXPECT_GT(math::InverseSqrt(std::numeric_limits<float>::min()), 1e-18f);
    EXPECT_GT(math::InverseSqrt(1e-10f), 1e4f);

    static_assert(math::InverseSqrt(0.0f) == std::numeric_limits<float>::infinity());
    static_assert(math::InverseSqrt(std::numeric_limits<float>::min()) > 1e-18f);
    static_assert(math::InverseSqrt(1e-10f) > 1e-4f);
}

TEST(MathUtilities, InverseSqrtNegativeInputBehavior) {
    EXPECT_TRUE(std::isinf(math::InverseSqrt(-1.0f)));

    static_assert(math::InverseSqrt(-1.0f) == std::numeric_limits<float>::infinity());
}

#pragma endregion

#pragma region Cos

TEST(MathUtilities, CosCommonAngles) {
    EXPECT_NEAR(math::Cos(0.0f), 1.0f, 1e-4f);
    EXPECT_NEAR(math::Cos(math::pi_over_2), 0.0f, 1e-4f);
    EXPECT_NEAR(math::Cos(math::pi), -1.0f, 1e-4f);
    EXPECT_NEAR(math::Cos(3.0f * math::pi_over_2), 0.0f, 1e-4f);
    EXPECT_NEAR(math::Cos(math::two_pi), 1.0f, 1e-4f);

    static_assert(ApproxEqual(math::Cos(0.0f), 1.0f));
    static_assert(ApproxEqual(math::Cos(math::pi_over_2), 0.0f));
    static_assert(ApproxEqual(math::Cos(math::pi), -1.0f));
    static_assert(ApproxEqual(math::Cos(3.0f * math::pi_over_2), 0.0f));
    static_assert(ApproxEqual(math::Cos(math::two_pi), 1.0f));
}

TEST(MathUtilities, CosNegativeAngles) {
    EXPECT_NEAR(math::Cos(-math::pi_over_2), 0.0f, 1e-4f);
    EXPECT_NEAR(math::Cos(-math::pi), -1.0f, 1e-4f);
    EXPECT_NEAR(math::Cos(-math::two_pi), 1.0f, 1e-4f);

    static_assert(ApproxEqual(math::Cos(-math::pi_over_2), 0.0f));
    static_assert(ApproxEqual(math::Cos(-math::pi), -1.0f));
    static_assert(ApproxEqual(math::Cos(-math::two_pi), 1.0f));
}

TEST(MathUtilities, CosSymmetryProperties) {
    constexpr float x = math::pi / 3.0f;
    EXPECT_NEAR(math::Cos(x), math::Cos(-x), 1e-4f);
    EXPECT_NEAR(math::Cos(math::pi - x), -math::Cos(x), 1e-4f);
    EXPECT_NEAR(math::Cos(math::pi + x), -math::Cos(x), 1e-4f);
    EXPECT_NEAR(math::Cos(math::two_pi - x), math::Cos(x), 1e-4f);

    static_assert(ApproxEqual(math::Cos(x), math::Cos(-x)));
    static_assert(ApproxEqual(math::Cos(math::pi - x), -math::Cos(x)));
    static_assert(ApproxEqual(math::Cos(math::pi + x), -math::Cos(x)));
    static_assert(ApproxEqual(math::Cos(math::two_pi - x), math::Cos(x)));
}

TEST(MathUtilities, CosPeriodicity) {
    constexpr float x = 1.23f;
    EXPECT_NEAR(math::Cos(x), math::Cos(x + math::two_pi), 1e-4f);
    EXPECT_NEAR(math::Cos(x), math::Cos(x - math::two_pi), 1e-4f);

    static_assert(ApproxEqual(math::Cos(x), math::Cos(x + math::two_pi)));
    static_assert(ApproxEqual(math::Cos(x), math::Cos(x - math::two_pi)));
}

#pragma endregion

#pragma region Sin

TEST(MathUtilities, SinCommonAngles) {
    EXPECT_NEAR(math::Sin(0.0f), 0.0f, 1e-4f);
    EXPECT_NEAR(math::Sin(math::pi_over_2), 1.0f, 1e-4f);
    EXPECT_NEAR(math::Sin(math::pi), 0.0f, 1e-4f);
    EXPECT_NEAR(math::Sin(3.0f * math::pi_over_2), -1.0f, 1e-4f);
    EXPECT_NEAR(math::Sin(math::two_pi), 0.0f, 1e-4f);

    static_assert(ApproxEqual(math::Sin(0.0f), 0.0f));
    static_assert(ApproxEqual(math::Sin(math::pi_over_2), 1.0f));
    static_assert(ApproxEqual(math::Sin(math::pi), 0.0f));
    static_assert(ApproxEqual(math::Sin(3.0f * math::pi_over_2), -1.0f));
    static_assert(ApproxEqual(math::Sin(math::two_pi), 0.0f));
}

TEST(MathUtilities, SinNegativeAngles) {
    EXPECT_NEAR(math::Sin(-math::pi_over_2), -1.0f, 1e-4f);
    EXPECT_NEAR(math::Sin(-math::pi), 0.0f, 1e-4f);
    EXPECT_NEAR(math::Sin(-math::two_pi), 0.0f, 1e-4f);

    static_assert(ApproxEqual(math::Sin(-math::pi_over_2), -1.0f));
    static_assert(ApproxEqual(math::Sin(-math::pi), 0.0f));
    static_assert(ApproxEqual(math::Sin(-math::two_pi), 0.0f));
}

TEST(MathUtilities, SinSymmetryProperties) {
    constexpr float x = math::pi / 3.0f;
    EXPECT_NEAR(math::Sin(x), -math::Sin(-x), 1e-4f);
    EXPECT_NEAR(math::Sin(math::pi - x), math::Sin(x), 1e-4f);
    EXPECT_NEAR(math::Sin(math::pi + x), -math::Sin(x), 1e-4f);
    EXPECT_NEAR(math::Sin(math::two_pi - x), -math::Sin(x), 1e-4f);

    static_assert(ApproxEqual(math::Sin(x), -math::Sin(-x)));
    static_assert(ApproxEqual(math::Sin(math::pi - x), math::Sin(x)));
    static_assert(ApproxEqual(math::Sin(math::pi + x), -math::Sin(x)));
    static_assert(ApproxEqual(math::Sin(math::two_pi - x), -math::Sin(x)));
}

TEST(MathUtilities, SinPeriodicity) {
    constexpr float x = 1.23f;
    EXPECT_NEAR(math::Sin(x), math::Sin(x + math::two_pi), 1e-4f);
    EXPECT_NEAR(math::Sin(x), math::Sin(x - math::two_pi), 1e-4f);

    static_assert(ApproxEqual(math::Sin(x), math::Sin(x + math::two_pi)));
    static_assert(ApproxEqual(math::Sin(x), math::Sin(x - math::two_pi), 1e-3f));
}

#pragma endregion

#pragma region Lerp

TEST(MathUtilities, LerpBasic) {
    static_assert(math::Lerp(0.0f, 1.0f, 0.5f) == 0.5f);
    static_assert(math::Lerp(0.0f, 1.0f, 0.0f) == 0.0f);
    static_assert(math::Lerp(0.0f, 1.0f, 1.0f) == 1.0f);

    EXPECT_FLOAT_EQ(math::Lerp(0.0f, 1.0f, 0.5f), 0.5f);
    EXPECT_FLOAT_EQ(math::Lerp(0.0f, 1.0f, 0.0f), 0.0f);
    EXPECT_FLOAT_EQ(math::Lerp(0.0f, 1.0f, 1.0f), 1.0f);
}

TEST(MathUtilities, LerpNegativeValues) {
    static_assert(math::Lerp(-1.0f, 1.0f, 0.5f) == 0.0f);
    static_assert(math::Lerp(-1.0f, -2.0f, 0.5f) == -1.5f);

    EXPECT_FLOAT_EQ(math::Lerp(-1.0f, 1.0f, 0.5f), 0.0f);
    EXPECT_FLOAT_EQ(math::Lerp(-1.0f, -2.0f, 0.5f), -1.5f);
}

TEST(MathUtilities, LerpOutOfRangeFactor) {
    static_assert(math::Lerp(0.0f, 1.0f, -0.5f) == -0.5f);
    static_assert(math::Lerp(0.0f, 1.0f, 1.5f) == 1.5f);

    EXPECT_FLOAT_EQ(math::Lerp(0.0f, 1.0f, -0.5f), -0.5f);
    EXPECT_FLOAT_EQ(math::Lerp(0.0f, 1.0f, 1.5f), 1.5f);
}

#pragma endregion

#pragma region Smoothstep

TEST(MathUtilities, SmoothstepBasicBehavior) {
    static_assert(math::Smoothstep(0.0f, 1.0f, 0.0f) == 0.0f);
    static_assert(math::Smoothstep(0.0f, 1.0f, 0.5f) == 0.5f);
    static_assert(math::Smoothstep(0.0f, 1.0f, 1.0f) == 1.0f);

    EXPECT_FLOAT_EQ(math::Smoothstep(0.0f, 1.0f, 0.0f), 0.0f);
    EXPECT_FLOAT_EQ(math::Smoothstep(0.0f, 1.0f, 0.5f), 0.5f);
    EXPECT_FLOAT_EQ(math::Smoothstep(0.0f, 1.0f, 1.0f), 1.0f);
}

TEST(MathUtilities, SmoothstepOutOfRangeInput) {
    static_assert(math::Smoothstep(10.0f, 20.0f, 5.0f) == 0.0f);
    static_assert(math::Smoothstep(10.0f, 20.0f, 25.0f) == 1.0f);

    EXPECT_FLOAT_EQ(math::Smoothstep(10.0f, 20.0f, 5.0f), 0.0f);
    EXPECT_FLOAT_EQ(math::Smoothstep(10.0f, 20.0f, 25.0f), 1.0f);
}

TEST(MathUtilities, SmoothstepDegenerateRangeReturnsBound) {
    static_assert(math::Smoothstep(2.0f, 2.0f, -5.0f) == 0.0f);
    static_assert(math::Smoothstep(2.0f, 2.0f, 2.0f) == 0.0f);
    static_assert(math::Smoothstep(2.0f, 2.0f, 10.0f) == 1.0f);

    EXPECT_FLOAT_EQ(math::Smoothstep(2.0f, 2.0f, -5.0f), 0.0f);
    EXPECT_FLOAT_EQ(math::Smoothstep(2.0f, 2.0f, 2.0f), 0.0f);
    EXPECT_FLOAT_EQ(math::Smoothstep(2.0f, 2.0f, 10.0f), 1.0f);
}

#pragma endregion

#pragma region Atan

TEST(MathUtilities, AtanCommonAngles) {
    EXPECT_NEAR(math::Atan(0.0f), 0.0f, 1e-4f);
    EXPECT_NEAR(math::Atan(1.0f), math::pi_over_4, 1e-4f);
    EXPECT_NEAR(math::Atan(-1.0f), -math::pi_over_4, 1e-4f);
    EXPECT_NEAR(math::Atan(std::numeric_limits<float>::infinity()), math::pi_over_2, 1e-4f);
    EXPECT_NEAR(math::Atan(-std::numeric_limits<float>::infinity()), -math::pi_over_2, 1e-4f);

    static_assert(ApproxEqual(math::Atan(0.0f), 0.0f));
    static_assert(ApproxEqual(math::Atan(1.0f), math::pi_over_4));
    static_assert(ApproxEqual(math::Atan(-1.0f), -math::pi_over_4));
}

TEST(MathUtilities, AtanSymmetryProperties) {
    constexpr float x = 0.75f;
    EXPECT_NEAR(math::Atan(x), -math::Atan(-x), 1e-4f);

    static_assert(ApproxEqual(math::Atan(x), -math::Atan(-x)));
}

TEST(MathUtilities, AtanLimits) {
    EXPECT_NEAR(math::Atan(1e6f), math::pi_over_2, 1e-4f);
    EXPECT_NEAR(math::Atan(-1e6f), -math::pi_over_2, 1e-4f);
}

TEST(MathUtilities, AtanMonotonicity) {
    constexpr float a = 0.5f;
    constexpr float b = 1.0f;
    constexpr float c = 2.0f;

    EXPECT_GT(math::Atan(b), math::Atan(a));
    EXPECT_GT(math::Atan(c), math::Atan(b));
    EXPECT_GT(math::Atan(a), math::Atan(-a));
}

#pragma endregion

#pragma region Atan2

TEST(MathUtilities, Atan2CommonAngles) {
    EXPECT_NEAR(math::Atan2(0.0f, 1.0f), 0.0f, 1e-4f);
    EXPECT_NEAR(math::Atan2(1.0f, 0.0f), math::pi_over_2, 1e-4f);
    EXPECT_NEAR(math::Atan2(0.0f, -1.0f), math::pi, 1e-4f);
    EXPECT_NEAR(math::Atan2(-1.0f, 0.0f), -math::pi_over_2, 1e-4f);
    EXPECT_NEAR(math::Atan2(1.0f, 1.0f), math::pi_over_4, 1e-4f);
    EXPECT_NEAR(math::Atan2(-1.0f, -1.0f), -3.0f * math::pi_over_4, 1e-4f);

    static_assert(ApproxEqual(math::Atan2(0.0f, 1.0f), 0.0f));
    static_assert(ApproxEqual(math::Atan2(1.0f, 0.0f), math::pi_over_2));
    static_assert(ApproxEqual(math::Atan2(0.0f, -1.0f), math::pi));
    static_assert(ApproxEqual(math::Atan2(-1.0f, 0.0f), -math::pi_over_2));
    static_assert(ApproxEqual(math::Atan2(1.0f, 1.0f), math::pi_over_4));
}

TEST(MathUtilities, Atan2SymmetryProperties) {
    constexpr float x = 0.75f;
    constexpr float y = 0.5f;

    EXPECT_NEAR(math::Atan2(y, x), -math::Atan2(-y, x), 1e-4f);
    static_assert(ApproxEqual(math::Atan2(y, x), -math::Atan2(-y, x)));
}

TEST(MathUtilities, Atan2DiagonalQuadrants) {
    EXPECT_NEAR(math::Atan2(1.0f, -1.0f), 3.0f * math::pi_over_4, 1e-4f);
    EXPECT_NEAR(math::Atan2(-1.0f, -1.0f), -3.0f * math::pi_over_4, 1e-4f);
    EXPECT_NEAR(math::Atan2(-1.0f, 1.0f), -math::pi_over_4, 1e-4f);

    static_assert(ApproxEqual(math::Atan2(1.0f, -1.0f), 3.0f * math::pi_over_4));
    static_assert(ApproxEqual(math::Atan2(-1.0f, -1.0f), -3.0f * math::pi_over_4));
    static_assert(ApproxEqual(math::Atan2(-1.0f, 1.0f), -math::pi_over_4));
}

TEST(MathUtilities, Atan2ZeroZeroReturnsZero) {
    EXPECT_NEAR(math::Atan2(0.0f, 0.0f), 0.0f, 1e-4f); // defined fallback
    static_assert(ApproxEqual(math::Atan2(0.0f, 0.0f), 0.0f));
}

#pragma endregion

#pragma region Asin

TEST(MathUtilities, AsinCommonAngles) {
    EXPECT_NEAR(math::Asin(0.0f), 0.0f, 1e-4f);
    EXPECT_NEAR(math::Asin(0.5f), math::pi_over_6, 1e-4f);
    EXPECT_NEAR(math::Asin(0.70710678f), math::pi_over_4, 1e-4f); // sin(π/4)
    EXPECT_NEAR(math::Asin(1.0f), math::pi_over_2, 1e-4f);
    EXPECT_NEAR(math::Asin(-0.5f), -math::pi_over_6, 1e-4f);
    EXPECT_NEAR(math::Asin(-1.0f), -math::pi_over_2, 1e-4f);

    static_assert(ApproxEqual(math::Asin(0.0f), 0.0f));
    static_assert(ApproxEqual(math::Asin(0.5f), math::pi_over_6));
    static_assert(ApproxEqual(math::Asin(1.0f), math::pi_over_2));
    static_assert(ApproxEqual(math::Asin(-0.5f), -math::pi_over_6));
    static_assert(ApproxEqual(math::Asin(-1.0f), -math::pi_over_2));
}

TEST(MathUtilities, AsinSymmetryProperties) {
    constexpr float x = 0.6f;

    EXPECT_NEAR(math::Asin(-x), -math::Asin(x), 1e-4f);
    static_assert(ApproxEqual(math::Asin(-x), -math::Asin(x)));
}

TEST(MathUtilities, AsinInverseProperty) {
    constexpr float x = 0.5f;
    const auto angle = math::Asin(x);
    EXPECT_NEAR(math::Sin(angle), x, 1e-4f);

    constexpr float y = -0.7f;
    const auto angle2 = math::Asin(y);
    EXPECT_NEAR(math::Sin(angle2), y, 1e-4f);
}

#pragma endregion

#pragma region Exp

TEST(MathUtilities, ExpCommonValues) {
    EXPECT_NEAR(math::Exp(0.0f), 1.0f, 1e-4f);
    EXPECT_NEAR(math::Exp(1.0f), 2.71828f, 1e-3f);
    EXPECT_NEAR(math::Exp(-1.0f), 0.367879f, 1e-4f);
    EXPECT_NEAR(math::Exp(2.0f), 7.38905f, 1e-3f);
    EXPECT_NEAR(math::Exp(-5.0f), 0.006737f, 1e-4f);

    static_assert(ApproxEqual(math::Exp(0.0f), 1.0f));
    static_assert(math::Exp(1.0f) > 2.71f && math::Exp(1.0f) < 2.72f);
    static_assert(math::Exp(-1.0f) > 0.36f && math::Exp(-1.0f) < 0.37f);
}

TEST(MathUtilities, ExpLimits) {
    EXPECT_EQ(math::Exp(-100.0f), 0.0f);
    EXPECT_TRUE(std::isinf(math::Exp(100.0f)));

    static_assert(math::Exp(-100.0f) == 0.0f);
    static_assert(math::Exp(100.0f) == std::numeric_limits<float>::infinity());
}

TEST(MathUtilities, ExpMonotonicity) {
    constexpr float a = 0.5f;
    constexpr float b = 1.0f;
    constexpr float c = 1.5f;

    EXPECT_GT(math::Exp(b), math::Exp(a));
    EXPECT_GT(math::Exp(c), math::Exp(b));
    EXPECT_GT(math::Exp(a), math::Exp(-a));

    static_assert(math::Exp(1.0f) > math::Exp(0.0f));
}

#pragma endregion

#pragma region Log

TEST(MathUtilities, LogCommonValues) {
    EXPECT_NEAR(math::Log(1.0f), 0.0f, 1e-4f);
    EXPECT_NEAR(math::Log(2.7182818f), 1.0f, 1e-4f); // ln(e)
    EXPECT_NEAR(math::Log(2.0f), 0.693147f, 1e-4f); // ln(2)
    EXPECT_NEAR(math::Log(10.0f), 2.302585f, 1e-4f); // ln(10)

    static_assert(ApproxEqual(math::Log(1.0f), 0.0f));
    static_assert(math::Log(2.0f) > 0.69f && math::Log(2.0f) < 0.70f);
}

TEST(MathUtilities, LogLimits) {
    EXPECT_TRUE(std::isinf(math::Log(0.0f)));
    EXPECT_LT(math::Log(0.0f), 0.0f);
    EXPECT_TRUE(std::isnan(math::Log(-1.0f)));

    static_assert(math::Log(0.0f) == -std::numeric_limits<float>::infinity());
}

TEST(MathUtilities, LogMonotonicity) {
    constexpr float a = 0.5f;
    constexpr float b = 1.0f;
    constexpr float c = 10.0f;

    EXPECT_GT(math::Log(b), math::Log(a));
    EXPECT_GT(math::Log(c), math::Log(b));

    constexpr float x = 1.23f;
    EXPECT_NEAR(math::Log(math::Exp(x)), x, 1e-3f);

    static_assert(math::Log(10.0f) > math::Log(1.0f));
}

#pragma endregion

#pragma region Pow

TEST(MathUtilities, PowCommonValues) {
    EXPECT_NEAR(math::Pow(2.0f, 3.0f), 8.0f, 1e-2f);
    EXPECT_NEAR(math::Pow(10.0f, 2.0f), 100.0f, 1e-1f);
    EXPECT_NEAR(math::Pow(9.0f, 0.5f), 3.0f, 1e-3f); // square root
    EXPECT_NEAR(math::Pow(4.0f, -0.5f), 0.5f, 1e-3f); // inverse square root

    static_assert(math::Pow(5.0f, 0.0f) == 1.0f);
    static_assert(math::Pow(0.0f, 0.0f) == 1.0f);
}

TEST(MathUtilities, PowEdgeCases) {
    EXPECT_FLOAT_EQ(math::Pow(0.0f, 5.0f), 0.0f);
    EXPECT_TRUE(std::isinf(math::Pow(0.0f, -1.0f)));
    EXPECT_TRUE(std::isnan(math::Pow(-2.0f, 2.0f)));

    static_assert(math::Pow(0.0f, 2.0f) == 0.0f);
    static_assert(math::Pow(0.0f, -2.0f) == std::numeric_limits<float>::infinity());
}

TEST(MathUtilities, PowMonotonicity) {
    EXPECT_GT(math::Pow(2.0f, 4.0f), math::Pow(2.0f, 3.0f));
    EXPECT_LT(math::Pow(0.5f, 2.0f), math::Pow(0.5f, 1.0f));
    EXPECT_NEAR(math::Pow(7.89f, 1.0f), 7.89f, 1e-3f);

    static_assert(math::Pow(2.0f, 2.0f) > math::Pow(2.0f, 1.0f));
}

#pragma endregion

#pragma region Clamp

TEST(MathUtilities, ClampBasicBehavior) {
    static_assert(math::Clamp(-1.0f, 0.0f, 1.0f) == 0.0f);
    static_assert(math::Clamp(0.5f, 0.0f, 1.0f) == 0.5f);
    static_assert(math::Clamp(2.0f, 0.0f, 1.0f) == 1.0f);

    EXPECT_FLOAT_EQ(math::Clamp(-1.0f, 0.0f, 1.0f), 0.0f);
    EXPECT_FLOAT_EQ(math::Clamp(0.5f, 0.0f, 1.0f), 0.5f);
    EXPECT_FLOAT_EQ(math::Clamp(2.0f, 0.0f, 1.0f), 1.0f);
}

TEST(MathUtilities, ClampDegenerateRangeReturnsBound) {
    static_assert(math::Clamp(-5.0f, 2.0f, 2.0f) == 2.0f);
    static_assert(math::Clamp( 2.0f, 2.0f, 2.0f) == 2.0f);
    static_assert(math::Clamp(10.0f, 2.0f, 2.0f) == 2.0f);

    EXPECT_FLOAT_EQ(math::Clamp(-5.0f, 2.0f, 2.0f), 2.0f);
    EXPECT_FLOAT_EQ(math::Clamp( 2.0f, 2.0f, 2.0f), 2.0f);
    EXPECT_FLOAT_EQ(math::Clamp(10.0f, 2.0f, 2.0f), 2.0f);
}

TEST(MathUtilities, ClampNanPassThrough) {
    // With this ternary implementation, if v is NaN then both comparisons evaluate
    // to false, so the function returns v unchanged (NaN is preserved).
    const float nan = std::numeric_limits<float>::quiet_NaN();
    EXPECT_TRUE(std::isnan(math::Clamp(nan, -1.0f, 1.0f)));
}

#pragma endregion

#pragma region Color Management

TEST(MathUtilities, SRGBToLinear) {
    // boundaries
    EXPECT_FLOAT_EQ(math::SRGBToLinear(0.0f), 0.0f);
    EXPECT_FLOAT_EQ(math::SRGBToLinear(1.0f), 1.0f);

    // known values
    EXPECT_NEAR(math::SRGBToLinear(0.5f), 0.214041f, 1e-6f);
    EXPECT_NEAR(math::SRGBToLinear(0.18f), 0.02721f, 1e-5f);

    // clamping
    EXPECT_FLOAT_EQ(math::SRGBToLinear(-1.0f), 0.0f);
    EXPECT_FLOAT_EQ(math::SRGBToLinear( 2.0f), 1.0f);
}

TEST(MathUtilities, LinearToSRGB) {
    // boundaries
    EXPECT_FLOAT_EQ(math::LinearToSRGB(0.0f), 0.0f);
    EXPECT_FLOAT_EQ(math::LinearToSRGB(1.0f), 1.0f);

    // known values
    EXPECT_NEAR(math::LinearToSRGB(0.214041f), 0.5f, 1e-6f);

    // clamping
    EXPECT_FLOAT_EQ(math::LinearToSRGB(-1.0f), 0.0f);
    EXPECT_FLOAT_EQ(math::LinearToSRGB( 2.0f), 1.0f);
}

TEST(MathUtilities, RoundTripSRGBLinearSRGB) {
    constexpr float kValues[] = {
        0.0f, 0.003f, 0.01f, 0.1f, 0.25f, 0.5f, 0.75f, 1.0f
    };

    for (float c : kValues) {
        const float linear = math::SRGBToLinear(c);
        const float srgb = math::LinearToSRGB(linear);
        EXPECT_NEAR(srgb, c, 1e-5f);
    }
}

#pragma endregion
