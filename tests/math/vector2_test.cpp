/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>
#include <test_helpers.hpp>

#include <cassert>

#include <vglx/math/vector2.hpp>

#pragma region Constructors

TEST(Vector2, ConstructorDefault) {
    constexpr auto v = vglx::Vector2 {};

    EXPECT_VEC2_EQ(v, {0.0f, 0.0f});

    static_assert(v == vglx::Vector2 {0.0f, 0.0f});
}

TEST(Vector2, ConstructorSingleParameter) {
    constexpr auto v = vglx::Vector2 {1.0f};

    EXPECT_VEC2_EQ(v, {1.0f, 1.0f});

    static_assert(v == vglx::Vector2 {1.0f, 1.0f});
}

TEST(Vector2, ConstructorParameterized) {
    constexpr auto v = vglx::Vector2 {1.0f, 2.0f};

    EXPECT_VEC2_EQ(v, {1.0f, 2.0f});

    static_assert(v == vglx::Vector2 {1.0f, 2.0f});
}

#pragma endregion

#pragma region Unit vectors

TEST(Vector2, UnitVectorAlongX) {
    constexpr auto v = vglx::Vector2::X();

    EXPECT_VEC2_EQ(v, {1.0f, 0.0f});

    static_assert(v == vglx::Vector2 {1.0f, 0.0f});
}

TEST(Vector2, UnitVectorAlongY) {
    constexpr auto v = vglx::Vector2::Y();

    EXPECT_VEC2_EQ(v, {0.0f, 1.0f});

    static_assert(v == vglx::Vector2 {0.0f, 1.0f});
}

TEST(Vector2, ZeroVector) {
    constexpr auto v = vglx::Vector2::Zero();

    EXPECT_VEC2_EQ(v, {0.0f, 0.0f});

    static_assert(v == vglx::Vector2 {0.0f, 0.0f});
}

#pragma endregion

#pragma region Component Access

TEST(Vector2, ComponentAccessDirect) {
    constexpr auto v = vglx::Vector2 {1.0f, 2.0f};

    EXPECT_FLOAT_EQ(v.x, 1.0f);
    EXPECT_FLOAT_EQ(v.y, 2.0f);

    static_assert(v.x == 1.0f);
    static_assert(v.y == 2.0f);
}

TEST(Vector2, ComponentAccessRandomAccessOperator) {
    constexpr auto v = vglx::Vector2 {1.0f, 2.0f};

    EXPECT_FLOAT_EQ(v[0], 1.0f);
    EXPECT_FLOAT_EQ(v[1], 2.0f);
    EXPECT_DEATH({ (void)v[3]; }, ".*i >= 0 && i < 2.*");

    static_assert(v[0] == 1.0f);
    static_assert(v[1] == 2.0f);
}

#pragma region Dot Product

TEST(Vector2, DotProductPositiveValues) {
    constexpr auto v1 = vglx::Vector2 {1.0f, 2.0f};
    constexpr auto v2 = vglx::Vector2 {3.0f, 4.0f};

    EXPECT_FLOAT_EQ(vglx::Dot(v1, v2), 11.0f);

    static_assert(vglx::Dot(v1, v2) == 11.0f);
}

TEST(Vector2, DotProductMixedValues) {
    constexpr auto v1 = vglx::Vector2 {-1.0f, 2.0f};
    constexpr auto v2 = vglx::Vector2 {3.0f, -4.0f};

    EXPECT_FLOAT_EQ(vglx::Dot(v1, v2), -11.0f);

    static_assert(vglx::Dot(v1, v2) == -11.0f);
}

TEST(Vector2, DotProductZeroVector) {
    constexpr auto v1 = vglx::Vector2 {0.0f, 0.0f};
    constexpr auto v2 = vglx::Vector2 {1.0f, 2.0f};

    EXPECT_FLOAT_EQ(vglx::Dot(v1, v2), 0.0f);

    static_assert(vglx::Dot(v1, v2) == 0.0f);
}

TEST(Vector2, DotProductPerpendicularVectors) {
    constexpr auto v1 = vglx::Vector2 {1.0f, 0.0f};
    constexpr auto v2 = vglx::Vector2 {0.0f, 1.0f};

    EXPECT_FLOAT_EQ(vglx::Dot(v1, v2), 0.0f);

    static_assert(vglx::Dot(v1, v2) == 0.0f);
}

TEST(Vector2, DotProductParallelVectors) {
    constexpr auto v1 = vglx::Vector2 {1.0f, 0.0f};
    constexpr auto v2 = vglx::Vector2 {1.0f, 0.0f};

    EXPECT_FLOAT_EQ(vglx::Dot(v1, v2), 1.0f);

    static_assert(vglx::Dot(v1, v2) == 1.0f);
}

#pragma endregion

#pragma region Length

TEST(Vector2, LengthPositiveValues) {
    constexpr auto v = vglx::Vector2 {3.0f, 4.0f};

    EXPECT_TRUE(ApproxEqual(v.Length(), 5.0f));

    static_assert(ApproxEqual(v.Length(), 5.0f));
}

TEST(Vector2, LengthNegativeValues) {
    constexpr auto v = vglx::Vector2 {-3.0f, -4.0f};

    EXPECT_TRUE(ApproxEqual(v.Length(), 5.0f));

    static_assert(ApproxEqual(v.Length(), 5.0f));
}

TEST(Vector2, LengthZeroVector) {
    constexpr auto v = vglx::Vector2 {0.0f, 0.0f};

    EXPECT_FLOAT_EQ(v.Length(), 0.0f);

    static_assert(v.Length() == 0.0f);
}

TEST(Vector2, LengthUnitVector) {
    constexpr auto v = vglx::Vector2 {1.0f, 0.0f};

    EXPECT_TRUE(ApproxEqual(v.Length(), 1.0f));

    static_assert(ApproxEqual(v.Length(), 1.0f));
}

TEST(Vector2, LengthSquarePositiveValues) {
    constexpr auto v = vglx::Vector2 {3.0f, 4.0f};

    EXPECT_FLOAT_EQ(v.LengthSquared(), 25.0f);

    static_assert(v.LengthSquared() == 25.0f);
}

TEST(Vector2, LengthSquareNegativeValues) {
    constexpr auto v = vglx::Vector2 {-3.0f, -4.0f};

    EXPECT_FLOAT_EQ(v.LengthSquared(), 25.0f);

    static_assert(v.LengthSquared() == 25.0f);
}

TEST(Vector2, LengthSquareZeroVector) {
    constexpr auto v = vglx::Vector2 {0.0f, 0.0f};

    EXPECT_FLOAT_EQ(v.LengthSquared(), 0.0f);

    static_assert(v.LengthSquared() == 0.0f);
}

TEST(Vector2, LengthSquareUnitVector) {
    constexpr auto v = vglx::Vector2 {1.0f, 0.0f};

    EXPECT_FLOAT_EQ(v.LengthSquared(), 1.0f);

    static_assert(v.LengthSquared() == 1.0f);
}

#pragma endregion

#pragma region Addition

TEST(Vector2, AdditionBasic) {
    constexpr auto v1 = vglx::Vector2 {1.0f, 2.0f};
    constexpr auto v2 = vglx::Vector2 {3.0f, 4.0f};

    EXPECT_VEC2_EQ(v1 + v2, {4.0f, 6.0f});

    static_assert(v1 + v2 == vglx::Vector2 {4.0f, 6.0f});
}

TEST(Vector2, AdditionZeroVector) {
    constexpr auto v1 = vglx::Vector2 {1.0f, 2.0f};
    constexpr auto zero = vglx::Vector2::Zero();

    EXPECT_VEC2_EQ(v1 + zero, {1.0f, 2.0f});

    static_assert(v1 + zero == vglx::Vector2 {1.0f, 2.0f});
}

TEST(Vector2, AdditionNegativeValues) {
    constexpr auto v1 = vglx::Vector2 {1.0f, 2.0f};
    constexpr auto v2 = vglx::Vector2 {-3.0f, -4.0f};

    EXPECT_VEC2_EQ(v1 + v2, {-2.0f, -2.0f});

    static_assert(v1 + v2 == vglx::Vector2 {-2.0f, -2.0f});
}

TEST(Vector2, AdditionAssignment) {
    auto v1 = vglx::Vector2 {1.0f, 2.0f};
    v1 += vglx::Vector2 {3.0f, 4.0f};

    EXPECT_VEC2_EQ(v1, {4.0f, 6.0f});

    // Compile-time check
    constexpr auto v2 = []() {
        auto v = vglx::Vector2 {1.0f, 2.0f};
        v += vglx::Vector2 {3.0f, 4.0f};
        return v;
    }();

    static_assert(v2 == vglx::Vector2 {4.0f, 6.0f});
}

#pragma endregion

#pragma region Subtraction

TEST(Vector2, SubtractionBasic) {
    constexpr auto v1 = vglx::Vector2 {5.0f, 6.0f};
    constexpr auto v2 = vglx::Vector2 {3.0f, 2.0f};

    EXPECT_VEC2_EQ(v1 - v2, {2.0f, 4.0f});

    static_assert(v1 - v2 == vglx::Vector2 {2.0f, 4.0f});
}

TEST(Vector2, SubtractionFromSelf) {
    constexpr auto v = vglx::Vector2 {9.0f, 8.0f};

    EXPECT_VEC2_EQ(v - v, {0.0f, 0.0f});

    static_assert(v - v == vglx::Vector2 {0.0f, 0.0f});
}

TEST(Vector2, SubtractionFromZeroVector) {
    constexpr auto v1 = vglx::Vector2::Zero();
    constexpr auto v2 = vglx::Vector2 {2.0f, 4.0f};

    EXPECT_VEC2_EQ(v1 - v2, {-2.0f, -4.0f});

    static_assert(v1 - v2 == vglx::Vector2 {-2.0f, -4.0f});
}

TEST(Vector2, SubtractionAssignment) {
    auto v1 = vglx::Vector2 {5.0f, 6.0f};
    v1 -= vglx::Vector2 {3.0f, 2.0f};

    EXPECT_VEC2_EQ(v1, {2.0f, 4.0f});

    // Compile-time check
    constexpr auto v2 = []() {
        auto v = vglx::Vector2 {5.0f, 6.0f};
        v -= vglx::Vector2 {3.0f, 2.0f};
        return v;
    }();

    static_assert(v2 == vglx::Vector2 {2.0f, 4.0f});
}

#pragma endregion

#pragma region Multiplication

TEST(Vector2, ScalarMultiplicationPositiveScalar) {
    constexpr auto v = vglx::Vector2 {1.0f, 2.0f};

    EXPECT_VEC2_EQ(v * 2.0f, {2.0f, 4.0f});
    EXPECT_VEC2_EQ(2.0f * v, {2.0f, 4.0f});

    static_assert(v * 2.0f == vglx::Vector2 {2.0f, 4.0f});
    static_assert(2.0f * v == vglx::Vector2 {2.0f, 4.0f});
}

TEST(Vector2, ScalarMultiplicationNegativeScalar) {
    constexpr auto v = vglx::Vector2 {1.0f, 2.0f};

    EXPECT_VEC2_EQ(v * -1.0f, {-1.0f, -2.0f});
    EXPECT_VEC2_EQ(-1.0f * v, {-1.0f, -2.0f});

    static_assert(v * -1.0f == vglx::Vector2 {-1.0f, -2.0f});
    static_assert(-1.0f * v == vglx::Vector2 {-1.0f, -2.0f});
}

TEST(Vector2, ScalarMultiplicationZeroScalar) {
    constexpr auto v = vglx::Vector2 {1.0f, 2.0f};

    EXPECT_VEC2_EQ(v * 0.0f, {0.0f, 0.0f});
    EXPECT_VEC2_EQ(0.0f * v, {0.0f, 0.0f});

    static_assert(v * 0.0f == vglx::Vector2 {0.0f, 0.0f});
    static_assert(0.0f * v == vglx::Vector2 {0.0f, 0.0f});
}

TEST(Vector2, ScalarMultiplicationIdentity) {
    constexpr auto v = vglx::Vector2 {1.0f, 2.0f};

    EXPECT_VEC2_EQ(v * 1.0f, {1.0f, 2.0f});
    EXPECT_VEC2_EQ(1.0f * v, {1.0f, 2.0f});

    static_assert(v * 1.0f == vglx::Vector2 {1.0f, 2.0f});
    static_assert(1.0f * v == vglx::Vector2 {1.0f, 2.0f});
}

TEST(Vector2, ScalarMultiplicationAssignment) {
    auto v1 = vglx::Vector2 {1.0f, 2.0f};
    v1 *= 2.0f;

    EXPECT_VEC2_EQ(v1, {2.0f, 4.0f});

    // Compile-time check
    constexpr auto v2 = []() {
        auto v = vglx::Vector2 {1.0f, 2.0f};
        v *= 2.0f;
        return v;
    }();

    static_assert(v2 == vglx::Vector2 {2.0f, 4.0f});
}

TEST(Vector2, VectorMultiplication) {
    constexpr auto v1 = vglx::Vector2 {1.0f, 2.0f};
    constexpr auto v2 = vglx::Vector2 {3.0f, 4.0f};

    EXPECT_VEC2_EQ(v1 * v2, {3.0f, 8.0f});

    static_assert(v1 * v2 == vglx::Vector2 {3.0f, 8.0f});
}

TEST(Vector2, VectorMultiplicationAssignment) {
    auto v1 = vglx::Vector2 {1.0f, 2.0f};
    v1 *= vglx::Vector2 {4.0f, 5.0f};

    EXPECT_VEC2_EQ(v1, {4.0f, 10.0f});

    // Compile-time check
    constexpr auto v2 = []() {
        auto v = vglx::Vector2 {1.0f, 2.0f};
        v *= vglx::Vector2 {4.0f, 5.0f};
        return v;
    }();

    static_assert(v2 == vglx::Vector2 {4.0f, 10.0f});
}

#pragma endregion


#pragma region Division

TEST(Vector2, ScalarDivisionPositiveScalar) {
    constexpr auto v = vglx::Vector2 {2.0f, 4.0f};

    EXPECT_VEC2_EQ(v / 2.0f, {1.0f, 2.0f});

    static_assert(v / 2.0f == vglx::Vector2 {1.0f, 2.0f});
}

TEST(Vector2, ScalarDivisionNegativeScalar) {
    constexpr auto v = vglx::Vector2 {-2.0f, -4.0f};

    EXPECT_VEC2_EQ(v / -2.0f, {1.0f, 2.0f});

    static_assert(v / -2.0f == vglx::Vector2 {1.0f, 2.0f});
}

TEST(Vector2, ScalarDivisionZeroScalar) {
    constexpr auto v = vglx::Vector2 {2.0f, -4.0f};
    constexpr auto inf = std::numeric_limits<float>::infinity();

    EXPECT_VEC2_EQ(v / 0.0f, {inf, -inf});
}

TEST(Vector2, ScalarDivisionIdentity) {
    constexpr auto v = vglx::Vector2 {1.0f, 2.0f};

    EXPECT_VEC2_EQ(v / 1.0f, {1.0f, 2.0f});

    static_assert(v / 1.0f == vglx::Vector2 {1.0f, 2.0f});
}

#pragma endregion

#pragma region MinMax

TEST(Vector2, ComponentwiseMinMax) {
    auto v1 = vglx::Vector2 {2, -1};
    auto v2 = vglx::Vector2 {2, -1};

    v1.Min({3, -2});
    EXPECT_VEC2_EQ(v1, {2, -2});

    v2.Max({3, -2});
    EXPECT_VEC2_EQ(v2, {3, -1});

    constexpr auto v3 = []() {
        auto v = vglx::Vector2 {2, -1};
        v.Min({3, -2});
        return v;
    }();
    static_assert(v3 == vglx::Vector2 {2, -2});

    constexpr auto v4 = []() {
        auto v = vglx::Vector2 {2, -1};
        v.Max({3, -2});
        return v;
    }();
    static_assert(v4 == vglx::Vector2 {3, -1});
}

#pragma endregion

#pragma region Normalize

TEST(Vector2, NormalizeBasic) {
    constexpr auto v = vglx::Vector2 {3.0f, 4.0f};

    EXPECT_VEC2_NEAR(vglx::Normalize(v), {0.6f, 0.8f}, 1e-4);

    static_assert(ApproxEqual(vglx::Normalize(v).x, 0.6f));
    static_assert(ApproxEqual(vglx::Normalize(v).y, 0.8f));
}

TEST(Vector2, NormalizeNormalizedVector) {
    constexpr auto v = vglx::Vector2 {0.6f, 0.8f};

    EXPECT_VEC2_NEAR(vglx::Normalize(v), {0.6f, 0.8f}, 1e-4);

    static_assert(ApproxEqual(vglx::Normalize(v).x, 0.6f));
    static_assert(ApproxEqual(vglx::Normalize(v).y, 0.8f));
}

TEST(Vector2, NormalizeZeroVector) {
    constexpr auto v = vglx::Vector2 {0.0f, 0.0f};

    EXPECT_VEC2_EQ(vglx::Normalize(v), {0.0f, 0.0f});

    static_assert(vglx::Normalize(v).x == 0.0f);
    static_assert(vglx::Normalize(v).y == 0.0f);
}

TEST(Vector2, NormalizeMemberBasic) {
    auto v1 = vglx::Vector2 {3.0f, 4.0f};
    v1.Normalize();

    EXPECT_VEC2_NEAR(vglx::Normalize(v1), {0.6f, 0.8f}, 1e-4);

    // Compile-time check
    constexpr auto v2 = []() {
        auto v = vglx::Vector2 {3.0f, 4.0f};
        v.Normalize();
        return v;
    }();

    static_assert(ApproxEqual(v2.x, 0.6f));
    static_assert(ApproxEqual(v2.y, 0.8f));
}

TEST(Vector2, NormalizeMemberNormalizedVector) {
    auto v1 = vglx::Vector2 {0.6f, 0.8f};
    v1.Normalize();

    EXPECT_VEC2_NEAR(vglx::Normalize(v1), {0.6f, 0.8f}, 1e-4);

    // Compile-time check
    constexpr auto v2 = []() {
        auto v = vglx::Vector2 {0.6f, 0.8f};
        v.Normalize();
        return v;
    }();

    static_assert(ApproxEqual(v2.x, 0.6f));
    static_assert(ApproxEqual(v2.y, 0.8f));
}

TEST(Vector2, NormalizedMemberZeroVector) {
    auto v1 = vglx::Vector2 {0.0f, 0.0f};
    v1.Normalize();

    EXPECT_VEC2_EQ(v1, {0.0f, 0.0f});
}

#pragma endregion

#pragma region Equality Operator

TEST(Vector2, EqualityOperator) {
    constexpr auto v1 = vglx::Vector2 {1.0f, 2.0f};
    constexpr auto v2 = vglx::Vector2 {1.0f, 2.0f};
    constexpr auto v3 = vglx::Vector2 {4.0f, 5.0f};

    EXPECT_TRUE(v1 == v2);
    EXPECT_FALSE(v1 == v3);

    static_assert(v1 == v2);
    static_assert(v1 != v3);
}

TEST(Vector2, InequalityOperator) {
    constexpr auto v1 = vglx::Vector2 {1.0f, 2.0f};
    constexpr auto v2 = vglx::Vector2 {1.0f, 2.0f};
    constexpr auto v3 = vglx::Vector2 {4.0f, 5.0f};

    EXPECT_FALSE(v1 != v2);
    EXPECT_TRUE(v1 != v3);

    static_assert(v1 == v2);
    static_assert(v1 != v3);
}

#pragma endregion

#pragma region Lerp

TEST(Vector2, Lerp) {
    constexpr auto v1 = vglx::Vector2 {0.0f, 0.0f};
    constexpr auto v2 = vglx::Vector2 {1.0f, 1.0f};

    EXPECT_VEC2_EQ(vglx::Lerp(v1, v2, 0.5f), {0.5f, 0.5f});

    static_assert(vglx::Lerp(v1, v2, 0.5f) == vglx::Vector2 {0.5f, 0.5f});
}

TEST(Vector2, LerpZeroFactor) {
    constexpr auto v1 = vglx::Vector2 {1.0f, 2.0f};
    constexpr auto v2 = vglx::Vector2 {3.0f, 4.0f};

    EXPECT_VEC2_EQ(vglx::Lerp(v1, v2, 0.0f), {1.0f, 2.0f});

    static_assert(vglx::Lerp(v1, v2, 0.0f) == vglx::Vector2 {1.0f, 2.0f});
}

TEST(Vector2, LerpOneFactor) {
    constexpr auto v1 = vglx::Vector2 {1.0f, 2.0f};
    constexpr auto v2 = vglx::Vector2 {3.0f, 4.0f};

    EXPECT_VEC2_EQ(vglx::Lerp(v1, v2, 1.0f), {3.0f, 4.0f});

    static_assert(vglx::Lerp(v1, v2, 1.0f) == vglx::Vector2 {3.0f, 4.0f});
}

#pragma endregion