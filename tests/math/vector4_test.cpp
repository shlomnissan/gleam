/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>
#include <test_helpers.hpp>

#include <cassert>
#include <limits>

#include <vglx/math/vector4.hpp>

#pragma region Constructors

TEST(Vector4, ConstructorDefault) {
    constexpr auto v = vglx::Vector4 {};

    EXPECT_VEC4_EQ(v, {0.0f, 0.0f, 0.0f, 0.0f});

    static_assert(v == vglx::Vector4 {0.0f, 0.0f, 0.0f, 0.0f});
}

TEST(Vector4, ConstructorSingleParameter) {
    constexpr auto v = vglx::Vector4 {1.0f};

    EXPECT_VEC4_EQ(v, {1.0f, 1.0f, 1.0f, 1.0f});

    static_assert(v == vglx::Vector4 {1.0f, 1.0f, 1.0f, 1.0f});
}

TEST(Vector4, ConstructorParameterized) {
    constexpr auto v = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0};

    EXPECT_VEC4_EQ(v, {1.0f, 2.0f, 3.0f, 4.0});

    static_assert(v == vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f});
}

#pragma endregion

#pragma region Unit vectors

TEST(Vector4, UnitVectorAlongX) {
    constexpr auto v = vglx::Vector4::X();

    EXPECT_VEC4_EQ(v, {1.0f, 0.0f, 0.0f, 0.0f});

    static_assert(v == vglx::Vector4 {1.0f, 0.0f, 0.0f, 0.0f});
}

TEST(Vector4, UnitVectorAlongY) {
    constexpr auto v = vglx::Vector4::Y();

    EXPECT_VEC4_EQ(v, {0.0f, 1.0f, 0.0f, 0.0f});

    static_assert(v == vglx::Vector4 {0.0f, 1.0f, 0.0f, 0.0f});
}

TEST(Vector4, UnitVectorAlongZ) {
    constexpr auto v = vglx::Vector4::Z();

    EXPECT_VEC4_EQ(v, {0.0f, 0.0f, 1.0f, 0.0f});

    static_assert(v == vglx::Vector4 {0.0f, 0.0f, 1.0f, 0.0f});
}

TEST(Vector4, UnitVectorAlongW) {
    constexpr auto v = vglx::Vector4::W();

    EXPECT_VEC4_EQ(v, {0.0f, 0.0f, 0.0f, 1.0f});

    static_assert(v == vglx::Vector4 {0.0f, 0.0f, 0.0f, 1.0f});
}

TEST(Vector4, ZeroVector) {
    constexpr auto v = vglx::Vector4::Zero();

    EXPECT_VEC4_EQ(v, {0.0f, 0.0f, 0.0f, 0.0f});

    static_assert(v == vglx::Vector4 {0.0f, 0.0f, 0.0f, 0.0f});
}

#pragma endregion

#pragma region Component Access

TEST(Vector4, ComponentAccessDirect) {
    constexpr auto v = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_FLOAT_EQ(v.x, 1.0f);
    EXPECT_FLOAT_EQ(v.y, 2.0f);
    EXPECT_FLOAT_EQ(v.z, 3.0f);
    EXPECT_FLOAT_EQ(v.w, 4.0f);

    static_assert(v.x == 1.0f);
    static_assert(v.y == 2.0f);
    static_assert(v.z == 3.0f);
    static_assert(v.w == 4.0f);
}

TEST(Vector4, ComponentAccessRandomAccessOperator) {
    constexpr auto v = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_FLOAT_EQ(v[0], 1.0f);
    EXPECT_FLOAT_EQ(v[1], 2.0f);
    EXPECT_FLOAT_EQ(v[2], 3.0f);
    EXPECT_FLOAT_EQ(v[3], 4.0f);
    EXPECT_DEATH({ (void)v[4]; }, ".*i >= 0 && i < 4.*");

    static_assert(v[0] == 1.0f);
    static_assert(v[1] == 2.0f);
    static_assert(v[2] == 3.0f);
    static_assert(v[3] == 4.0f);
}

#pragma endregion

#pragma region Dot Product

TEST(Vector4, DotProductPositiveValues) {
    constexpr auto v1 = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f};
    constexpr auto v2 = vglx::Vector4 {4.0f, 5.0f, 6.0f, 7.0f};

    EXPECT_FLOAT_EQ(vglx::Dot(v1, v2), 60.0f);

    static_assert(vglx::Dot(v1, v2) == 60.0f);
}

TEST(Vector4, DotProductMixedValues) {
    constexpr auto v1 = vglx::Vector4 {-1.0f, 2.0f, -3.0f, 4.0f};
    constexpr auto v2 = vglx::Vector4 {4.0f, -5.0f, 6.0f, -7.0f};

    EXPECT_FLOAT_EQ(vglx::Dot(v1, v2), -60.0f);

    static_assert(vglx::Dot(v1, v2) == -60.0f);
}

TEST(Vector4, DotProductZeroVector) {
    constexpr auto v1 = vglx::Vector4 {0.0f, 0.0f, 0.0f, 0.0f};
    constexpr auto v2 = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_FLOAT_EQ(vglx::Dot(v1, v2), 0.0f);

    static_assert(vglx::Dot(v1, v2) == 0.0f);
}

TEST(Vector4, DotProductPerpendicularVectors) {
    constexpr auto v1 = vglx::Vector4 {1.0f, 0.0f, 0.0f, 0.0f};
    constexpr auto v2 = vglx::Vector4 {0.0f, 1.0f, 0.0f, 0.0f};

    EXPECT_FLOAT_EQ(vglx::Dot(v1, v2), 0.0f);

    static_assert(vglx::Dot(v1, v2) == 0.0f);
}

TEST(Vector4, DotProductParallelVectors) {
    constexpr auto v1 = vglx::Vector4 {1.0f, 0.0f, 0.0f, 0.0f};
    constexpr auto v2 = vglx::Vector4 {1.0f, 0.0f, 0.0f, 0.0f};

    EXPECT_FLOAT_EQ(vglx::Dot(v1, v2), 1.0f);

    static_assert(vglx::Dot(v1, v2) == 1.0f);
}

#pragma endregion

#pragma region Length

TEST(Vector4, LengthPositiveValues) {
    constexpr auto v = vglx::Vector4 {3.0f, 4.0f, 0.0f, 0.0f};

    EXPECT_TRUE(ApproxEqual(v.Length(), 5.0f));

    static_assert(ApproxEqual(v.Length(), 5.0f));
}

TEST(Vector4, LengthNegativeValues) {
    constexpr auto v = vglx::Vector4 {-3.0f, -4.0f, 0.0f, 0.0f};

    EXPECT_TRUE(ApproxEqual(v.Length(), 5.0f));

    static_assert(ApproxEqual(v.Length(), 5.0f));
}

TEST(Vector4, LengthZeroVector) {
    constexpr auto v = vglx::Vector4 {0.0f, 0.0f, 0.0f, 0.0f};

    EXPECT_FLOAT_EQ(v.Length(), 0.0f);

    static_assert(v.Length() == 0.0f);
}

TEST(Vector4, LengthUnitVector) {
    constexpr auto v = vglx::Vector4 {1.0f, 0.0f, 0.0f, 0.0f};

    EXPECT_TRUE(ApproxEqual(v.Length(), 1.0f));

    static_assert(ApproxEqual(v.Length(), 1.0f));
}

TEST(Vector4, LengthSquaredPositiveValues) {
    constexpr auto v = vglx::Vector4 {3.0f, 4.0f, 0.0f, 0.0f};

    EXPECT_FLOAT_EQ(v.LengthSquared(), 25.0f);

    static_assert(v.LengthSquared() == 25.0f);
}

TEST(Vector4, LengthSquaredNegativeValues) {
    constexpr auto v = vglx::Vector4 {-3.0f, -4.0f, 0.0f, 0.0f};

    EXPECT_FLOAT_EQ(v.LengthSquared(), 25.0f);

    static_assert(v.LengthSquared() == 25.0f);
}

TEST(Vector4, LengthSquaredZeroVector) {
    constexpr auto v = vglx::Vector4 {0.0f, 0.0f, 0.0f, 0.0f};

    EXPECT_FLOAT_EQ(v.LengthSquared(), 0.0f);

    static_assert(v.LengthSquared() == 0.0f);
}

TEST(Vector4, LengthSquaredUnitVector) {
    constexpr auto v = vglx::Vector4 {1.0f, 0.0f, 0.0f, 0.0f};

    EXPECT_FLOAT_EQ(v.LengthSquared(), 1.0f);

    static_assert(v.LengthSquared() == 1.0f);
}

#pragma endregion

#pragma region Addition

TEST(Vector4, AdditionBasic) {
    constexpr auto v1 = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f};
    constexpr auto v2 = vglx::Vector4 {5.0f, 6.0f, 7.0f, 8.0f};

    EXPECT_VEC4_EQ(v1 + v2, {6.0f, 8.0f, 10.0f, 12.0f});

    static_assert(v1 + v2 == vglx::Vector4 {6.0f, 8.0f, 10.0f, 12.0f});
}

TEST(Vector4, AdditionZeroVector) {
    constexpr auto v1 = vglx::Vector4 {9.0f, -1.0f, 2.0f, -4.0f};
    constexpr auto v2 = vglx::Vector4 {0.0f, 0.0f, 0.0f, 0.0f};

    EXPECT_VEC4_EQ(v1 + v2, {9.0f, -1.0f, 2.0f, -4.0f});

    static_assert(v1 + v2 == vglx::Vector4 {9.0f, -1.0f, 2.0f, -4.0f});
}

TEST(Vector4, AdditionNegativeValues) {
    constexpr auto v1 = vglx::Vector4 {-1.0f, -2.0f, -3.0f, -4.0f};
    constexpr auto v2 = vglx::Vector4 {-5.0f, -6.0f, -7.0f, -8.0f};

    EXPECT_VEC4_EQ(v1 + v2, {-6.0f, -8.0f, -10.0f, -12.0f});

    static_assert(v1 + v2 == vglx::Vector4 {-6.0f, -8.0f, -10.0f, -12.0f});
}

TEST(Vector4, AdditionAssignment) {
    auto v1 = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f};
    v1 += vglx::Vector4 {5.0f, 6.0f, 7.0f, 8.0f};

    EXPECT_VEC4_EQ(v1, {6.0f, 8.0f, 10.0f, 12.0f});

    // Compile-time check
    constexpr auto v2 = []() {
        auto v = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f};
        v += vglx::Vector4 {5.0f, 6.0f, 7.0f, 8.0f};
        return v;
    }();

    static_assert(v2 == vglx::Vector4 {6.0f, 8.0f, 10.0f, 12.0f});
}

#pragma endregion

#pragma region Subtraction

TEST(Vector4, SubtractionBasic) {
    constexpr auto v1 = vglx::Vector4 {10.0f, 9.0f, 8.0f, 7.0f};
    constexpr auto v2 = vglx::Vector4 {4.0f, 3.0f, 2.0f, 1.0f};

    EXPECT_VEC4_EQ(v1 - v2, {6.0f, 6.0f, 6.0f, 6.0f});

    static_assert(v1 - v2 == vglx::Vector4 {6.0f, 6.0f, 6.0f, 6.0f});
}

TEST(Vector4, SubtractionFromSelf) {
    constexpr auto v = vglx::Vector4 {3.0f, 5.0f, 7.0f, 9.0f};

    EXPECT_VEC4_EQ(v - v, {0.0f, 0.0f, 0.0f, 0.0f});

    static_assert(v - v == vglx::Vector4 {0.0f, 0.0f, 0.0f, 0.0f});
}

TEST(Vector4, SubtractionFromZeroVector) {
    constexpr auto v1 = vglx::Vector4 {0.0f, 0.0f, 0.0f, 0.0f};
    constexpr auto v2 = vglx::Vector4 {2.0f, 4.0f, 6.0f, 8.0f};

    EXPECT_VEC4_EQ(v1 - v2, {-2.0f, -4.0f, -6.0f, -8.0f});

    static_assert(v1 - v2 == vglx::Vector4 {-2.0f, -4.0f, -6.0f, -8.0f});
}

TEST(Vector4, SubtractionAssignment) {
    auto v1 = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f};
    v1 -= vglx::Vector4 {4.0f, 5.0f, 6.0f, 7.0f};

    EXPECT_VEC4_EQ(v1, {-3.0f, -3.0f, -3.0f, -3.0f});

    // Compile-time check
    constexpr auto v2 = []() {
        auto v = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f};
        v -= vglx::Vector4 {4.0f, 5.0f, 6.0f, 7.0f};
        return v;
    }();

    static_assert(v2 == vglx::Vector4 {-3.0f, -3.0f, -3.0f, -3.0f});
}

#pragma endregion

#pragma region Multiplication

TEST(Vector4, ScalarMultiplicationPositiveScalar) {
    constexpr auto v = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_VEC4_EQ(v * 2.0f, {2.0f, 4.0f, 6.0f, 8.0f});
    EXPECT_VEC4_EQ(2.0f * v, {2.0f, 4.0f, 6.0f, 8.0f});

    static_assert(v * 2.0f == vglx::Vector4 {2.0f, 4.0f, 6.0f, 8.0f});
    static_assert(2.0f * v == vglx::Vector4 {2.0f, 4.0f, 6.0f, 8.0f});
}

TEST(Vector4, ScalarMultiplicationNegativeScalar) {
    constexpr auto v = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_VEC4_EQ(v * -1.0f, {-1.0f, -2.0f, -3.0f, -4.0f});
    EXPECT_VEC4_EQ(-1.0f * v, {-1.0f, -2.0f, -3.0f, -4.0f});

    static_assert(v * -1.0f == vglx::Vector4 {-1.0f, -2.0f, -3.0f, -4.0f});
    static_assert(-1.0f * v == vglx::Vector4 {-1.0f, -2.0f, -3.0f, -4.0f});
}

TEST(Vector4, ScalarMultiplicationZeroScalar) {
    constexpr auto v = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_VEC4_EQ(v * 0.0f, {0.0f, 0.0f, 0.0f, 0.0f});
    EXPECT_VEC4_EQ(0.0f * v, {0.0f, 0.0f, 0.0f, 0.0f});

    static_assert(v * 0.0f == vglx::Vector4 {0.0f, 0.0f, 0.0f, 0.0f});
    static_assert(0.0f * v == vglx::Vector4 {0.0f, 0.0f, 0.0f, 0.0f});
}

TEST(Vector4, ScalarMultiplicationIdentity) {
    constexpr auto v = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_VEC4_EQ(v * 1.0f, {1.0f, 2.0f, 3.0f, 4.0f});
    EXPECT_VEC4_EQ(1.0f * v, {1.0f, 2.0f, 3.0f, 4.0f});

    static_assert(v * 1.0f == vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f});
    static_assert(1.0f * v == vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f});
}

TEST(Vector4, ScalarMultiplicationAssignment) {
    auto v1 = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f};
    v1 *= 2.0f;

    EXPECT_VEC4_EQ(v1, {2.0f, 4.0f, 6.0f, 8.0f});

    // Compile-time check
    constexpr auto v2 = []() {
        auto v = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f};
        v *= 2.0f;
        return v;
    }();

    static_assert(v2 == vglx::Vector4 {2.0f, 4.0f, 6.0f, 8.0f});
}

TEST(Vector4, VectorMultiplicationAssignment) {
    auto v1 = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f};
    v1 *= vglx::Vector4 {4.0f, 5.0f, 6.0f, 7.0f};

    EXPECT_VEC4_EQ(v1, {4.0f, 10.0f, 18.0f, 28.0f});

    // Compile-time check
    constexpr auto v2 = []() {
        auto v = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f};
        v *= vglx::Vector4 {4.0f, 5.0f, 6.0f, 7.0f};
        return v;
    }();

    static_assert(v2 == vglx::Vector4 {4.0f, 10.0f, 18.0f, 28.0f});
}

TEST(Vector4, VectorMultiplication) {
    constexpr auto v1 = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f};
    constexpr auto v2 = vglx::Vector4 {4.0f, 5.0f, 6.0f, 7.0f};

    EXPECT_VEC4_EQ(v1 * v2, {4.0f, 10.0f, 18.0f, 28.0f});

    static_assert(v1 * v2 == vglx::Vector4 {4.0f, 10.0f, 18.0f, 28.0f});
}

#pragma endregion

#pragma region Division

TEST(Vector4, ScalarDivisionPositiveScalar) {
    constexpr auto v = vglx::Vector4 {2.0f, 4.0f, 6.0f, 8.0f};

    EXPECT_VEC4_EQ(v / 2.0f, {1.0f, 2.0f, 3.0f, 4.0f});

    static_assert(v / 2.0f == vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f});
}

TEST(Vector4, ScalarDivisionNegativeScalar) {
    constexpr auto v = vglx::Vector4 {-2.0f, -4.0f, -6.0f, -8.0f};

    EXPECT_VEC4_EQ(v / -2.0f, {1.0f, 2.0f, 3.0f, 4.0f});

    static_assert(v / -2.0f == vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f});
}

TEST(Vector4, ScalarDivisionZeroScalar) {
    constexpr auto v = vglx::Vector4 {2.0f, -4.0f, 6.0f, -8.0f};
    constexpr auto inf = std::numeric_limits<float>::infinity();

    EXPECT_VEC4_EQ(v / 0.0f, {inf, -inf, inf, -inf});
}

TEST(Vector4, ScalarDivisionIdentity) {
    constexpr auto v = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_VEC4_EQ(v / 1.0f, {1.0f, 2.0f, 3.0f, 4.0f});

    static_assert(v / 1.0f == vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f});
}

#pragma endregion

#pragma region MinMax

TEST(Vector4, ComponentwiseMinMax) {
    auto v1 = vglx::Vector4 {2, -1, 5, 1};
    auto v2 = vglx::Vector4 {2, -1, 5, 1};

    v1.Min({3, -2, 4, -1});
    EXPECT_VEC4_EQ(v1, {2, -2, 4, -1});

    v2.Max({3, -2, 4, 7});
    EXPECT_VEC4_EQ(v2, {3, -1, 5, 7});

    constexpr auto v3 = []() {
        auto v = vglx::Vector4 {2, -1, 5, 1};
        v.Min({3, -2, 4, -1});
        return v;
    }();
    static_assert(v3 == vglx::Vector4 {2, -2, 4, -1});

    constexpr auto v4 = []() {
        auto v = vglx::Vector4 {2, -1, 5, 1};
        v.Max({3, -2, 4, 7});
        return v;
    }();
    static_assert(v4 == vglx::Vector4 {3, -1, 5, 7});
}

#pragma endregion

#pragma region Normalize

TEST(Vector4, NormalizeBasic) {
    constexpr auto v = vglx::Vector4 {3.0f, 4.0f, 0.0f, 0.0f};

    EXPECT_VEC4_NEAR(vglx::Normalize(v), {0.6f, 0.8f, 0.0f, 0.0f}, 1e-4);

    static_assert(ApproxEqual(vglx::Normalize(v).x, 0.6f));
    static_assert(ApproxEqual(vglx::Normalize(v).y, 0.8f));
    static_assert(ApproxEqual(vglx::Normalize(v).z, 0.0f));
    static_assert(ApproxEqual(vglx::Normalize(v).z, 0.0f));
}

TEST(Vector4, NormalizeNormalizedVector) {
    constexpr auto v = vglx::Vector4 {0.6f, 0.8f, 0.0f, 0.0f};

    EXPECT_VEC4_NEAR(vglx::Normalize(v), {0.6f, 0.8f, 0.0f, 0.0f}, 1e-4);

    static_assert(ApproxEqual(vglx::Normalize(v).x, 0.6f));
    static_assert(ApproxEqual(vglx::Normalize(v).y, 0.8f));
    static_assert(ApproxEqual(vglx::Normalize(v).z, 0.0f));
    static_assert(ApproxEqual(vglx::Normalize(v).z, 0.0f));
}

TEST(Vector4, NormalizeZeroVector) {
    constexpr auto v = vglx::Vector4 {0.0f, 0.0f, 0.0f, 0.0f};

    EXPECT_VEC4_EQ(vglx::Normalize(v), {0.0f, 0.0f, 0.0f, 0.0f});

    static_assert(ApproxEqual(vglx::Normalize(v).x, 0.0f));
    static_assert(ApproxEqual(vglx::Normalize(v).y, 0.0f));
    static_assert(ApproxEqual(vglx::Normalize(v).z, 0.0f));
    static_assert(ApproxEqual(vglx::Normalize(v).z, 0.0f));
}

TEST(Vector4, NormalizedMemberBasic) {
    auto v1 = vglx::Vector4 {3.0f, 4.0f, 0.0f, 0.0f};
    v1.Normalize();

    EXPECT_VEC4_NEAR(v1, {0.6f, 0.8f, 0.0f, 0.0f}, 1e-4);

    // Compile-time check
    constexpr auto v2 = []() {
        auto v = vglx::Vector4 {3.0f, 4.0f, 0.0f, 0.0f};
        v.Normalize();
        return v;
    }();

    static_assert(ApproxEqual(v2.x, 0.6f));
    static_assert(ApproxEqual(v2.y, 0.8f));
    static_assert(ApproxEqual(v2.z, 0.0f));
    static_assert(ApproxEqual(v2.w, 0.0f));
}

TEST(Vector4, NormalizedMemberNormalizedVector) {
    auto v1 = vglx::Vector4 {0.6f, 0.8f, 0.0f, 0.0f};
    v1.Normalize();

    EXPECT_VEC4_NEAR(v1, {0.6f, 0.8f, 0.0f, 0.0f}, 1e-4);

    // Compile-time check
    constexpr auto v2 = []() {
        auto v = vglx::Vector4 {0.6f, 0.8f, 0.0f, 0.0f};
        v.Normalize();
        return v;
    }();

    static_assert(ApproxEqual(v2.x, 0.6f));
    static_assert(ApproxEqual(v2.y, 0.8f));
    static_assert(ApproxEqual(v2.z, 0.0f));
    static_assert(ApproxEqual(v2.w, 0.0f));
}

TEST(Vector4, NormalizedMemberZeroVector) {
    auto v = vglx::Vector4 {0.0f, 0.0f, 0.0f, 0.0f};
    v.Normalize();

    EXPECT_VEC4_EQ(v, {0.0f, 0.0f, 0.0f, 0.0f});
}

#pragma endregion

#pragma region Equality Operator

TEST(Vector4, EqualityOperator) {
    constexpr auto v1 = vglx::Vector4 {1.0f, 2.0f, 3.0f, 1.0f};
    constexpr auto v2 = vglx::Vector4 {1.0f, 2.0f, 3.0f, 1.0f};
    constexpr auto v3 = vglx::Vector4 {4.0f, 5.0f, 6.0f, 1.0f};

    EXPECT_TRUE(v1 == v2);
    EXPECT_FALSE(v1 == v3);

    static_assert(v1 == v2);
    static_assert(v1 != v3);
}

TEST(Vector4, InequalityOperator) {
    constexpr auto v1 = vglx::Vector4 {1.0f, 2.0f, 3.0f, 1.0f};
    constexpr auto v2 = vglx::Vector4 {1.0f, 2.0f, 3.0f, 1.0f};
    constexpr auto v3 = vglx::Vector4 {4.0f, 5.0f, 6.0f, 1.0f};

    EXPECT_FALSE(v1 != v2);
    EXPECT_TRUE(v1 != v3);

    static_assert(v1 == v2);
    static_assert(v1 != v3);
}

#pragma endregion

#pragma region Lerp

TEST(Vector4, Lerp) {
    constexpr auto v1 = vglx::Vector4 {0.0f, 0.0f, 0.0f, 0.0f};
    constexpr auto v2 = vglx::Vector4 {1.0f, 1.0f, 1.0f, 1.0f};

    EXPECT_VEC4_EQ(vglx::Lerp(v1, v2, 0.5f), {0.5f, 0.5f, 0.5f, 0.5f});

    static_assert(
        vglx::Lerp(v1, v2, 0.5f) == vglx::Vector4 {0.5f, 0.5f, 0.5f, 0.5f}
    );
}

TEST(Vector4, LerpZeroFactor) {
    constexpr auto v1 = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f};
    constexpr auto v2 = vglx::Vector4 {5.0f, 6.0f, 7.0f, 8.0f};

    EXPECT_VEC4_EQ(vglx::Lerp(v1, v2, 0.0f), {1.0f, 2.0f, 3.0f, 4.0f});

    static_assert(
        vglx::Lerp(v1, v2, 0.0f) == vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f}
    );
}

TEST(Vector4, LerpOneFactor) {
    constexpr auto v1 = vglx::Vector4 {1.0f, 2.0f, 3.0f, 4.0f};
    constexpr auto v2 = vglx::Vector4 {5.0f, 6.0f, 7.0f, 8.0f};

    EXPECT_VEC4_EQ(vglx::Lerp(v1, v2, 1.0f), {5.0f, 6.0f, 7.0f, 8.0f});

    static_assert(
        vglx::Lerp(v1, v2, 1.0f) == vglx::Vector4 {5.0f, 6.0f, 7.0f, 8.0f}
    );
}

#pragma endregion