/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>
#include <test_helpers.hpp>

#include <cassert>

#include <vglx/math/color.hpp>

auto EXPECT_COLOR_EQ(const vglx::Color& a, const vglx::Color& b) {
    EXPECT_FLOAT_EQ(a.r, b.r);
    EXPECT_FLOAT_EQ(a.g, b.g);
    EXPECT_FLOAT_EQ(a.b, b.b);
}

auto EXPECT_COLOR_NEAR(const vglx::Color& a, const vglx::Color& b, float v) {
    EXPECT_NEAR(a.r, b.r, v);
    EXPECT_NEAR(a.g, b.g, v);
    EXPECT_NEAR(a.b, b.b, v);
}

#pragma region Constructors

TEST(Color, ConstructorDefault) {
    auto c = vglx::Color {};

    EXPECT_COLOR_EQ(c, 0xFFFFFF);
}

TEST(Color, ConstructorRGB) {
    constexpr auto c = vglx::Color {0.5f, 0.25f, 0.75f};

    EXPECT_COLOR_EQ(c, {0.5f, 0.25f, 0.75f});

    static_assert(c == vglx::Color {0.5f, 0.25f, 0.75f});
}

TEST(Color, DISABLED_ConstructorHex) {
    auto c = vglx::Color {0xFF7F50};

    EXPECT_COLOR_NEAR(c, {1.0f, 0.4f, 0.3f}, 0.1f);
}

#pragma endregion

#pragma region Component Access

TEST(DISABLED_Color, ComponentAccessDirect) {
    auto c = vglx::Color {0xFF7F50};

    EXPECT_NEAR(c.r, 1.0f, 0.1f);
    EXPECT_NEAR(c.g, 0.4f, 0.1f);
    EXPECT_NEAR(c.b, 0.3f, 0.1f);
}

TEST(DISABLED_Color, ComponentAccessRandomAccessOperator) {
    auto c = vglx::Color {0xFF7F50};

    EXPECT_NEAR(c[0], 1.0f, 0.1f);
    EXPECT_NEAR(c[1], 0.4f, 0.1f);
    EXPECT_NEAR(c[2], 0.3f, 0.1f);
}

#pragma endregion

#pragma region Assignment Operator

TEST(Color, AssignmentOperatorHex) {
    auto c1 = vglx::Color {0.1f, 0.1f, 0.1f};
    c1 = 0xFF4500;

    EXPECT_COLOR_NEAR(c1, {1.0f, 0.27f, 0.0f}, 0.1f);
}

#pragma endregion

#pragma region Equality Operator

TEST(Color, EqualityOperator) {
    auto c1 = vglx::Color {0xFFAD69};
    auto c2 = vglx::Color {0xFFAD69};
    auto c3 = vglx::Color {0x47A8BD};

    EXPECT_TRUE(c1 == c2);
    EXPECT_FALSE(c1 == c3);
}

TEST(Color, InequalityOperator) {
    auto c1 = vglx::Color {0xFFAD69};
    auto c2 = vglx::Color {0xFFAD69};
    auto c3 = vglx::Color {0x47A8BD};

    EXPECT_FALSE(c1 != c2);
    EXPECT_TRUE(c1 != c3);
}

#pragma endregion

#pragma region Addition

TEST(Color, AdditionBasic) {
    auto c1 = vglx::Color {0.2f, 0.2f, 0.4f};
    auto c2 = vglx::Color {0.1f, 0.1f, 0.1f};

    EXPECT_COLOR_EQ(c1 + c2, {0.3f, 0.3f, 0.5f});
}

TEST(Color, AdditionBlackColor) {
    auto c = vglx::Color {0.2f, 0.4f, 0.6f};
    auto black = vglx::Color {0x000000};

    EXPECT_COLOR_EQ(c + black, {0.2f, 0.4f, 0.6f});
}

#pragma endregion

#pragma region Subtraction

TEST(Color, SubtractionBasic) {
    auto c1 = vglx::Color {0.2f, 0.8f, 0.4f};
    auto c2 = vglx::Color {0.2f, 0.4f, 0.1f};

    EXPECT_COLOR_EQ(c1 - c2, {0.0f, 0.4f, 0.3f});
}

TEST(Color, SubtractionWithBlackColor) {
    auto c = vglx::Color {0.5f, 0.7f, 0.9f};
    auto black = vglx::Color {0x000000};

    EXPECT_COLOR_EQ(c - black, {0.5f, 0.7f, 0.9f});
}

TEST(Color, SubtractionFromSelf) {
    auto c = vglx::Color {0.5f, 0.7f, 0.9f};

    EXPECT_COLOR_EQ(c - c, 0x000000);
}

TEST(Color, SubtractionResultingInClamping) {
    auto c1 = vglx::Color {0.1f, 0.2f, 0.3f};
    auto c2 = vglx::Color {0.2f, 0.3f, 0.4f};
    auto result = c1 - c2;

    EXPECT_COLOR_EQ(result, {0.0f, 0.0f, 0.0f});
}

#pragma endregion

#pragma region Multiplication

TEST(Color, ScalarMultiplication) {
    auto c = vglx::Color {0.2f, 0.4f, 0.6f} * 2.0f;

    EXPECT_COLOR_EQ(c, {0.4f, 0.8f, 1.2f});
}


TEST(Color, ScalarMultiplicationInPlace) {
    auto c1 = vglx::Color {0.2f, 0.4f, 0.6f};
    c1 *= 2.0f;

    EXPECT_COLOR_EQ(c1, {0.4f, 0.8f, 1.2f});
}

#pragma endregion

#pragma region Lerp

TEST(Color, Lerp) {
    auto c1 = vglx::Color {0.0f, 0.0f, 0.0f};
    auto c2 = vglx::Color {1.0f, 1.0f, 1.0f};

    EXPECT_COLOR_EQ(vglx::Lerp(c1, c2, 0.5f), {0.5f, 0.5f, 0.5f});
}

TEST(Color, LerpZeroFactor) {
    auto c1 = vglx::Color {0.5f, 0.5f, 0.5f};
    auto c2 = vglx::Color {1.0f, 1.0f, 1.0f};

    EXPECT_COLOR_EQ(vglx::Lerp(c1, c2, 0.0f), c1);
}

TEST(Color, LerpOneFactor) {
    auto c1 = vglx::Color {0.5f, 0.5f, 0.5f};
    auto c2 = vglx::Color {1.0f, 1.0f, 1.0f};

    EXPECT_COLOR_EQ(vglx::Lerp(c1, c2, 1.0f), c2);
}

#pragma endregion