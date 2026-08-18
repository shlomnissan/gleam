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

constexpr auto LINEAR_TO_SRGB(const vglx::Color& c) {
    return vglx::Color {
        vglx::math::LinearToSRGB(c.r),
        vglx::math::LinearToSRGB(c.g),
        vglx::math::LinearToSRGB(c.b)
    };
}

#pragma region Constructors

TEST(Color, ConstructorDefault) {
    constexpr auto c = vglx::Color {};

    EXPECT_COLOR_EQ(c, 0xFFFFFFu);

    static_assert(c == vglx::Color {0xFFFFFFu});
}

TEST(Color, ConstructorRGB) {
    constexpr auto c = vglx::Color {0.5f, 0.25f, 0.75f};

    EXPECT_COLOR_EQ(c, {0.5f, 0.25f, 0.75f});

    static_assert(c == vglx::Color {0.5f, 0.25f, 0.75f});
}

TEST(Color, ConstructorHex) {
    constexpr auto c = LINEAR_TO_SRGB(vglx::Color {0xFF7F50u});

    EXPECT_COLOR_NEAR(c, {1.0f, 0.4f, 0.3f}, 0.1f);

    static_assert(ApproxEqual(c.r, 1.0f, 0.1f));
    static_assert(ApproxEqual(c.g, 0.4f, 0.1f));
    static_assert(ApproxEqual(c.b, 0.3f, 0.1f));
}

#pragma endregion

#pragma region Component Access

TEST(Color, ComponentAccessDirect) {
    constexpr auto c = LINEAR_TO_SRGB(vglx::Color {0xFF7F50u});

    EXPECT_NEAR(c.r, 1.0f, 0.1f);
    EXPECT_NEAR(c.g, 0.4f, 0.1f);
    EXPECT_NEAR(c.b, 0.3f, 0.1f);

    static_assert(ApproxEqual(c.r, 1.0f, 0.1f));
    static_assert(ApproxEqual(c.g, 0.4f, 0.1f));
    static_assert(ApproxEqual(c.b, 0.3f, 0.1f));
}

TEST(Color, ComponentAccessRandomAccessOperator) {
    constexpr auto c = LINEAR_TO_SRGB(vglx::Color {0xFF7F50u});

    EXPECT_NEAR(c[0], 1.0f, 0.1f);
    EXPECT_NEAR(c[1], 0.4f, 0.1f);
    EXPECT_NEAR(c[2], 0.3f, 0.1f);

    static_assert(ApproxEqual(c.r, 1.0f, 0.1f));
    static_assert(ApproxEqual(c.g, 0.4f, 0.1f));
    static_assert(ApproxEqual(c.b, 0.3f, 0.1f));
}

#pragma endregion

#pragma region Assignment Operator

TEST(Color, AssignmentOperatorHex) {
    auto c1 = vglx::Color {0.1f, 0.1f, 0.1f};
    c1 = 0xFF4500u;

    EXPECT_COLOR_NEAR(c1, {1.0f, 0.27f, 0.0f}, 0.1f);
}

#pragma endregion

#pragma region Equality Operator

TEST(Color, EqualityOperator) {
    constexpr auto c1 = vglx::Color {0xFFAD69u};
    constexpr auto c2 = vglx::Color {0xFFAD69u};
    constexpr auto c3 = vglx::Color {0x47A8BDu};

    EXPECT_TRUE(c1 == c2);
    EXPECT_FALSE(c1 == c3);

    static_assert(c1 == c2);
    static_assert(c1 != c3);
}

TEST(Color, InequalityOperator) {
    constexpr auto c1 = vglx::Color {0xFFAD69u};
    constexpr auto c2 = vglx::Color {0xFFAD69u};
    constexpr auto c3 = vglx::Color {0x47A8BDu};

    EXPECT_FALSE(c1 != c2);
    EXPECT_TRUE(c1 != c3);

    static_assert(c1 == c2);
    static_assert(c1 != c3);
}

#pragma endregion

#pragma region Addition

TEST(Color, AdditionBasic) {
    constexpr auto c1 = vglx::Color {0.2f, 0.2f, 0.4f};
    constexpr auto c2 = vglx::Color {0.1f, 0.1f, 0.1f};

    EXPECT_COLOR_EQ(c1 + c2, {0.3f, 0.3f, 0.5f});

    static_assert(c1 + c2 == vglx::Color {0.3f, 0.3f, 0.5f});
}

TEST(Color, AdditionBlackColor) {
    constexpr auto c = vglx::Color {0.2f, 0.4f, 0.6f};
    constexpr auto black = vglx::Color {0x000000u};

    EXPECT_COLOR_EQ(c + black, {0.2f, 0.4f, 0.6f});

    static_assert(c + black == vglx::Color {0.2f, 0.4f, 0.6f});
}

#pragma endregion

#pragma region Subtraction

TEST(Color, SubtractionBasic) {
    constexpr auto c1 = vglx::Color {0.2f, 0.8f, 0.4f};
    constexpr auto c2 = vglx::Color {0.2f, 0.4f, 0.1f};

    EXPECT_COLOR_EQ(c1 - c2, {0.0f, 0.4f, 0.3f});

    static_assert(c1 - c2 == vglx::Color {0.0f, 0.4f, 0.3f});
}

TEST(Color, SubtractionWithBlackColor) {
    constexpr auto c = vglx::Color {0.5f, 0.7f, 0.9f};
    constexpr auto black = vglx::Color {0x000000u};

    EXPECT_COLOR_EQ(c - black, {0.5f, 0.7f, 0.9f});

    static_assert(c - black == vglx::Color {0.5f, 0.7f, 0.9f});
}

TEST(Color, SubtractionFromSelf) {
    constexpr auto c = vglx::Color {0.5f, 0.7f, 0.9f};

    EXPECT_COLOR_EQ(c - c, 0x000000u);

    static_assert(c - c == vglx::Color {0x000000u});
}

TEST(Color, SubtractionResultingInClamping) {
    constexpr auto c1 = vglx::Color {0.1f, 0.2f, 0.3f};
    constexpr auto c2 = vglx::Color {0.2f, 0.3f, 0.4f};
    constexpr auto result = c1 - c2;

    EXPECT_COLOR_EQ(result, {0.0f, 0.0f, 0.0f});

    static_assert(result == vglx::Color {0.0f, 0.0f, 0.0f});
}

#pragma endregion

#pragma region Multiplication

TEST(Color, ScalarMultiplication) {
    constexpr auto c = vglx::Color {0.2f, 0.4f, 0.6f} * 2.0f;

    EXPECT_COLOR_EQ(c, {0.4f, 0.8f, 1.2f});

    static_assert(c == vglx::Color {0.4f, 0.8f, 1.2f});
}


TEST(Color, ScalarMultiplicationInPlace) {
    auto c1 = vglx::Color {0.2f, 0.4f, 0.6f};
    c1 *= 2.0f;

    EXPECT_COLOR_EQ(c1, {0.4f, 0.8f, 1.2f});

    // Compile-time check
    constexpr auto c2 = []() {
        auto c = vglx::Color {0.2f, 0.4f, 0.6f};
        return c * 2.0f;
    }();

    static_assert(c2 == vglx::Color {0.4f, 0.8f, 1.2f});
}

TEST(Color, ComponentWiseMultiplication) {
    constexpr auto c1 = vglx::Color {0.2f, 0.4f, 0.6f};
    constexpr auto c2 = vglx::Color {0.5f, 2.0f, 0.0f};

    constexpr auto result = c1 * c2;

    EXPECT_COLOR_EQ(result, {0.1f, 0.8f, 0.0f});

    static_assert(result == vglx::Color {0.1f, 0.8f, 0.0f});
}

TEST(Color, ComponentWiseMultiplicationInPlace) {
    auto c1 = vglx::Color {0.2f, 0.4f, 0.6f};
    c1 *= vglx::Color {0.5f, 2.0f, 0.0f};

    EXPECT_COLOR_EQ(c1, {0.1f, 0.8f, 0.0f});

    // Compile-time check
    constexpr auto c2 = []() {
        auto c = vglx::Color {0.2f, 0.4f, 0.6f};
        c *= vglx::Color {0.5f, 2.0f, 0.0f};
        return c;
    }();

    static_assert(c2 == vglx::Color {0.1f, 0.8f, 0.0f});
}

#pragma endregion

#pragma region Lerp

TEST(Color, Lerp) {
    constexpr auto c1 = vglx::Color {0.0f, 0.0f, 0.0f};
    constexpr auto c2 = vglx::Color {1.0f, 1.0f, 1.0f};

    EXPECT_COLOR_EQ(vglx::Lerp(c1, c2, 0.5f), {0.5f, 0.5f, 0.5f});

    static_assert(vglx::Lerp(c1, c2, 0.5f) == vglx::Color {0.5f, 0.5f, 0.5f});
}

TEST(Color, LerpZeroFactor) {
    constexpr auto c1 = vglx::Color {0.5f, 0.5f, 0.5f};
    constexpr auto c2 = vglx::Color {1.0f, 1.0f, 1.0f};

    EXPECT_COLOR_EQ(vglx::Lerp(c1, c2, 0.0f), c1);

    static_assert(vglx::Lerp(c1, c2, 0.0f) == c1);
}

TEST(Color, LerpOneFactor) {
    constexpr auto c1 = vglx::Color {0.5f, 0.5f, 0.5f};
    constexpr auto c2 = vglx::Color {1.0f, 1.0f, 1.0f};

    EXPECT_COLOR_EQ(vglx::Lerp(c1, c2, 1.0f), c2);

    static_assert(vglx::Lerp(c1, c2, 1.0f) == c2);
}

#pragma endregion