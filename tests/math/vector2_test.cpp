/*
===========================================================================
  GLEAM ENGINE https://gleamengine.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>
#include <test_helpers.hpp>

#include <cassert>

#include <gleam/math/vector2.hpp>

#pragma region Constructors

TEST(Vector2, ConstructorDefault) {
    constexpr auto v = gleam::Vector2 {};

    EXPECT_VEC2_EQ(v, {0.0f, 0.0f});

    static_assert(v == gleam::Vector2 {0.0f, 0.0f});
}

TEST(Vector2, ConstructorSingleParameter) {
    constexpr auto v = gleam::Vector2 {1.0f};

    EXPECT_VEC2_EQ(v, {1.0f, 1.0f});

    static_assert(v == gleam::Vector2 {1.0f, 1.0f});
}

TEST(Vector2, ConstructorParameterized) {
    constexpr auto v = gleam::Vector2 {1.0f, 2.0f};

    EXPECT_VEC2_EQ(v, {1.0f, 2.0f});

    static_assert(v == gleam::Vector2 {1.0f, 2.0f});
}

#pragma endregion

#pragma region Component Access

TEST(Vector2, ComponentAccessDirect) {
    const auto v = gleam::Vector2 {1.0f, 2.0f};

    EXPECT_FLOAT_EQ(v.x, 1.0f);
    EXPECT_FLOAT_EQ(v.y, 2.0f);
}

TEST(Vector2, ComponentAccessRandomAccessOperator) {
    const auto v = gleam::Vector2 {1.0f, 2.0f};

    EXPECT_FLOAT_EQ(v[0], 1.0f);
    EXPECT_FLOAT_EQ(v[1], 2.0f);
    EXPECT_DEATH({ (void)v[3]; }, ".*i >= 0 && i < 2.*");
}

#pragma endregion

#pragma region Addition

TEST(Vector2, AdditionBasic) {
    constexpr auto v1 = gleam::Vector2 {1.0f, 2.0f};
    constexpr auto v2 = gleam::Vector2 {3.0f, 4.0f};

    EXPECT_VEC2_EQ(v1 + v2, {4.0f, 6.0f});

    static_assert(v1 + v2 == gleam::Vector2 {4.0f, 6.0f});
}

TEST(Vector2, AdditionZeroVector) {
    constexpr auto v1 = gleam::Vector2 {1.0f, 2.0f};
    constexpr auto zero = gleam::Vector2::Zero();

    EXPECT_VEC2_EQ(v1 + zero, {1.0f, 2.0f});

    static_assert(v1 + zero == gleam::Vector2 {1.0f, 2.0f});
}

TEST(Vector2, AdditionNegativeValues) {
    constexpr auto v1 = gleam::Vector2 {1.0f, 2.0f};
    constexpr auto v2 = gleam::Vector2 {-3.0f, -4.0f};

    EXPECT_VEC2_EQ(v1 + v2, {-2.0f, -2.0f});

    static_assert(v1 + v2 == gleam::Vector2 {-2.0f, -2.0f});
}

TEST(Vector2, AdditionAssignment) {
    auto v1 = gleam::Vector2 {1.0f, 2.0f};
    v1 += gleam::Vector2 {3.0f, 4.0f};

    EXPECT_VEC2_EQ(v1, {4.0f, 6.0f});

    // Compile-time check
    constexpr auto v2 = []() {
        auto v = gleam::Vector2 {1.0f, 2.0f};
        v += gleam::Vector2 {3.0f, 4.0f};
        return v;
    }();

    static_assert(v2 == gleam::Vector2 {4.0f, 6.0f});
}

#pragma endregion

#pragma region Subtraction

TEST(Vector2, SubtractionBasic) {
    constexpr auto v1 = gleam::Vector2 {5.0f, 6.0f};
    constexpr auto v2 = gleam::Vector2 {3.0f, 2.0f};

    EXPECT_VEC2_EQ(v1 - v2, {2.0f, 4.0f});

    static_assert(v1 - v2 == gleam::Vector2 {2.0f, 4.0f});
}

TEST(Vector2, SubtractionFromSelf) {
    constexpr auto v = gleam::Vector2 {9.0f, 8.0f};

    EXPECT_VEC2_EQ(v - v, {0.0f, 0.0f});

    static_assert(v - v == gleam::Vector2 {0.0f, 0.0f});
}

TEST(Vector2, SubtractionFromZeroVector) {
    constexpr auto v1 = gleam::Vector2::Zero();
    constexpr auto v2 = gleam::Vector2 {2.0f, 4.0f};

    EXPECT_VEC2_EQ(v1 - v2, {-2.0f, -4.0f});

    static_assert(v1 - v2 == gleam::Vector2 {-2.0f, -4.0f});
}

TEST(Vector2, SubtractionAssignment) {
    auto v1 = gleam::Vector2 {5.0f, 6.0f};
    v1 -= gleam::Vector2 {3.0f, 2.0f};

    EXPECT_VEC2_EQ(v1, {2.0f, 4.0f});

    // Compile-time check
    constexpr auto v2 = []() {
        auto v = gleam::Vector2 {5.0f, 6.0f};
        v -= gleam::Vector2 {3.0f, 2.0f};
        return v;
    }();

    static_assert(v2 == gleam::Vector2 {2.0f, 4.0f});
}

#pragma endregion

#pragma region Multiplication

TEST(Vector2, ScalarMultiplicationAssignment) {
    auto v1 = gleam::Vector2 {1.0f, 2.0f};
    v1 *= 2.0f;

    EXPECT_VEC2_EQ(v1, {2.0f, 4.0f});

    // Compile-time check
    constexpr auto v2 = []() {
        auto v = gleam::Vector2 {1.0f, 2.0f};
        v *= 2.0f;
        return v;
    }();

    static_assert(v2 == gleam::Vector2 {2.0f, 4.0f});
}

TEST(Vector2, VectorMultiplicationAssignment) {
    auto v1 = gleam::Vector2 {1.0f, 2.0f};
    v1 *= gleam::Vector2 {4.0f, 5.0f};

    EXPECT_VEC2_EQ(v1, {4.0f, 10.0f});

    // Compile-time check
    constexpr auto v2 = []() {
        auto v = gleam::Vector2 {1.0f, 2.0f};
        v *= gleam::Vector2 {4.0f, 5.0f};
        return v;
    }();

    static_assert(v2 == gleam::Vector2 {4.0f, 10.0f});
}

#pragma endregion

#pragma region Equality Operator

TEST(Vector2, EqualityOperator) {
    constexpr auto v1 = gleam::Vector2 {1.0f, 2.0f};
    constexpr auto v2 = gleam::Vector2 {1.0f, 2.0f};
    constexpr auto v3 = gleam::Vector2 {4.0f, 5.0f};

    EXPECT_TRUE(v1 == v2);
    EXPECT_FALSE(v1 == v3);

    static_assert(v1 == v2);
    static_assert(v1 != v3);
}

TEST(Vector2, InequalityOperator) {
    constexpr auto v1 = gleam::Vector2 {1.0f, 2.0f};
    constexpr auto v2 = gleam::Vector2 {1.0f, 2.0f};
    constexpr auto v3 = gleam::Vector2 {4.0f, 5.0f};

    EXPECT_FALSE(v1 != v2);
    EXPECT_TRUE(v1 != v3);

    static_assert(v1 == v2);
    static_assert(v1 != v3);
}

#pragma endregion