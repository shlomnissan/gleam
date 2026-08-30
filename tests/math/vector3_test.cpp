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

#include <vglx/math/vector3.hpp>

#pragma region Constructors

TEST(Vector3, ConstructorDefault) {
    constexpr auto v = vglx::Vector3 {};

    EXPECT_VEC3_EQ(v, {0.0f, 0.0f, 0.0f});

    static_assert(v == vglx::Vector3 {0.0f, 0.0f, 0.0f});
}

TEST(Vector3, ConstructorSingleParameter) {
    constexpr auto v = vglx::Vector3 {1.0f};

    EXPECT_VEC3_EQ(v, {1.0f, 1.0f, 1.0f});

    static_assert(v == vglx::Vector3 {1.0f, 1.0f, 1.0f});
}

TEST(Vector3, ConstructorParameterized) {
    constexpr auto v = vglx::Vector3 {1.0f, 2.0f, 3.0f};

    EXPECT_VEC3_EQ(v, {1.0f, 2.0f, 3.0f});

    static_assert(v == vglx::Vector3 {1.0f, 2.0f, 3.0f});
}

#pragma endregion

#pragma region Unit vectors

TEST(Vector3, UnitVectorAlongX) {
    constexpr auto v = vglx::Vector3::UnitX();

    EXPECT_VEC3_EQ(v, {1.0, 0.0, 0.0});

    static_assert(v == vglx::Vector3 {1.0f, 0.0f, 0.0f});
}

TEST(Vector3, UnitVectorAlongY) {
    constexpr auto v = vglx::Vector3::UnitY();

    EXPECT_VEC3_EQ(v, {0.0, 1.0, 0.0});

    static_assert(v == vglx::Vector3 {0.0f, 1.0f, 0.0f});
}

TEST(Vector3, UnitVectorAlongZ) {
    constexpr auto v = vglx::Vector3::UnitZ();

    EXPECT_VEC3_EQ(v, {0.0, 0.0, 1.0});

    static_assert(v == vglx::Vector3 {0.0f, 0.0f, 1.0f});
}

TEST(Vector3, ZeroVector) {
    constexpr auto v = vglx::Vector3::Zero();

    EXPECT_VEC3_EQ(v, {0.0, 0.0, 0.0});

    static_assert(v == vglx::Vector3 {0.0f, 0.0f, 0.0f});
}

#pragma endregion

#pragma region Component Access

TEST(Vector3, ComponentAccessDirect) {
    constexpr auto v = vglx::Vector3 {1.0f, 2.0f, 3.0f};

    EXPECT_FLOAT_EQ(v.x, 1.0f);
    EXPECT_FLOAT_EQ(v.y, 2.0f);
    EXPECT_FLOAT_EQ(v.z, 3.0f);

    static_assert(v.x == 1.0f);
    static_assert(v.y == 2.0f);
    static_assert(v.z == 3.0f);
}

TEST(Vector3, ComponentAccessRandomAccessOperator) {
    constexpr auto v = vglx::Vector3 {1.0f, 2.0f, 3.0f};

    EXPECT_FLOAT_EQ(v[0], 1.0f);
    EXPECT_FLOAT_EQ(v[1], 2.0f);
    EXPECT_FLOAT_EQ(v[2], 3.0f);

    static_assert(v[0] == 1.0f);
    static_assert(v[1] == 2.0f);
    static_assert(v[2] == 3.0f);
}

#pragma endregion

#pragma region Cross Product

TEST(Vector3, CrossProductPositiveValues) {
    constexpr auto v1 = vglx::Vector3 {1.0f, 2.0f, 3.0f};
    constexpr auto v2 = vglx::Vector3 {4.0f, 5.0f, 6.0f};

    EXPECT_VEC3_EQ(vglx::Cross(v1, v2), {-3.0f, 6.0f, -3.0f});

    static_assert(vglx::Cross(v1, v2) == vglx::Vector3 {-3.0f, 6.0f, -3.0f});
}

TEST(Vector3, CrossProductZeroVector) {
    constexpr auto v1 = vglx::Vector3 {1.0f, 2.0f, 3.0f};
    constexpr auto v2 = vglx::Vector3 {0.0f, 0.0f, 0.0f};

    EXPECT_VEC3_EQ(vglx::Cross(v1, v2), {0.0f, 0.0f, 0.0f});

    static_assert(vglx::Cross(v1, v2) == vglx::Vector3 {0.0f, 0.0f, 0.0f});
}

TEST(Vector3, CrossProductParallelVectors) {
    constexpr auto v1 = vglx::Vector3 {1.0f, 2.0f, 3.0f};
    constexpr auto v2 = vglx::Vector3 {2.0f, 4.0f, 6.0f};

    EXPECT_VEC3_EQ(vglx::Cross(v1, v2), {0.0f, 0.0f, 0.0f});

    static_assert(vglx::Cross(v1, v2) == vglx::Vector3 {0.0f, 0.0f, 0.0f});
}

TEST(Vector3, CrossProductUnitVectors) {
    constexpr auto v1 = vglx::Vector3 {1.0f, 0.0f, 0.0f};
    constexpr auto v2 = vglx::Vector3 {0.0f, 1.0f, 0.0f};

    EXPECT_VEC3_EQ(vglx::Cross(v1, v2), {0.0f, 0.0f, 1.0f});

    static_assert(vglx::Cross(v1, v2) == vglx::Vector3 {0.0f, 0.0f, 1.0f});
}

#pragma endregion

#pragma region Dot Product

TEST(Vector3, DotProductPositiveValues) {
    constexpr auto v1 = vglx::Vector3 {1.0f, 2.0f, 3.0f};
    constexpr auto v2 = vglx::Vector3 {4.0f, 5.0f, 6.0f};

    EXPECT_FLOAT_EQ(vglx::Dot(v1, v2), 32.0f);

    static_assert(vglx::Dot(v1, v2) == 32.0f);
}

TEST(Vector3, DotProductMixedValues) {
    constexpr auto v1 = vglx::Vector3 {-1.0f, 2.0f, -3.0f};
    constexpr auto v2 = vglx::Vector3 {4.0f, -5.0f, 6.0f};

    EXPECT_FLOAT_EQ(vglx::Dot(v1, v2), -32.0f);

    static_assert(vglx::Dot(v1, v2) == -32.0f);
}

TEST(Vector3, DotProductZeroVector) {
    constexpr auto v1 = vglx::Vector3 {0.0f, 0.0f, 0.0f};
    constexpr auto v2 = vglx::Vector3 {1.0f, 2.0f, 3.0f};

    EXPECT_FLOAT_EQ(vglx::Dot(v1, v2), 0.0f);

    static_assert(vglx::Dot(v1, v2) == 0.0f);
}

TEST(Vector3, DotProductPerpendicularVectors) {
    constexpr auto v1 = vglx::Vector3 {1.0f, 0.0f, 0.0f};
    constexpr auto v2 = vglx::Vector3 {0.0f, 1.0f, 0.0f};

    EXPECT_FLOAT_EQ(vglx::Dot(v1, v2), 0.0f);

    static_assert(vglx::Dot(v1, v2) == 0.0f);
}

TEST(Vector3, DotProductParallelVectors) {
    constexpr auto v1 = vglx::Vector3 {1.0f, 0.0f, 0.0f};
    constexpr auto v2 = vglx::Vector3 {1.0f, 0.0f, 0.0f};

    EXPECT_FLOAT_EQ(vglx::Dot(v1, v2), 1.0f);

    static_assert(vglx::Dot(v1, v2) == 1.0f);
}

#pragma endregion

#pragma region Length

TEST(Vector3, LengthPositiveValues) {
    constexpr auto v = vglx::Vector3 {3.0f, 4.0f, 0.0f};

    EXPECT_TRUE(ApproxEqual(v.Length(), 5.0f));

    static_assert(ApproxEqual(v.Length(), 5.0f));
}

TEST(Vector3, LengthNegativeValues) {
    constexpr auto v = vglx::Vector3 {-3.0f, -4.0f, 0.0f};

    EXPECT_TRUE(ApproxEqual(v.Length(), 5.0f));

    static_assert(ApproxEqual(v.Length(), 5.0f));
}

TEST(Vector3, LengthZeroVector) {
    constexpr auto v = vglx::Vector3 {0.0f, 0.0f, 0.0f};

    EXPECT_FLOAT_EQ(v.Length(), 0.0f);

    static_assert(v.Length() == 0.0f);
}

TEST(Vector3, LengthUnitVector) {
    constexpr auto v = vglx::Vector3 {1.0f, 0.0f, 0.0f};

    EXPECT_TRUE(ApproxEqual(v.Length(), 1.0f));

    static_assert(ApproxEqual(v.Length(), 1.0f));
}

TEST(Vector3, LengthSquarePositiveValues) {
    constexpr auto v = vglx::Vector3 {3.0f, 4.0f, 0.0f};

    EXPECT_FLOAT_EQ(v.LengthSquared(), 25.0f);

    static_assert(v.LengthSquared() == 25.0f);
}

TEST(Vector3, LengthSquareNegativeValues) {
    constexpr auto v = vglx::Vector3 {-3.0f, -4.0f, 0.0f};

    EXPECT_FLOAT_EQ(v.LengthSquared(), 25.0f);

    static_assert(v.LengthSquared() == 25.0f);
}

TEST(Vector3, LengthSquareZeroVector) {
    constexpr auto v = vglx::Vector3 {0.0f, 0.0f, 0.0f};

    EXPECT_FLOAT_EQ(v.LengthSquared(), 0.0f);

    static_assert(v.LengthSquared() == 0.0f);
}

TEST(Vector3, LengthSquareUnitVector) {
    constexpr auto v = vglx::Vector3 {1.0f, 0.0f, 0.0f};

    EXPECT_FLOAT_EQ(v.LengthSquared(), 1.0f);

    static_assert(v.LengthSquared() == 1.0f);
}

#pragma endregion

#pragma region Addition

TEST(Vector3, AdditionBasic) {
    constexpr auto v1 = vglx::Vector3 {1.0f, 2.0f, 3.0f};
    constexpr auto v2 = vglx::Vector3 {4.0f, 5.0f, 6.0f};

    EXPECT_VEC3_EQ(v1 + v2, {5.0f, 7.0f, 9.0f});

    static_assert(v1 + v2 == vglx::Vector3 {5.0f, 7.0f, 9.0f});
}

TEST(Vector3, AdditionZeroVector) {
    constexpr auto v1 = vglx::Vector3 {7.0f, -3.0f, 2.0f};
    constexpr auto v2 = vglx::Vector3 {0.0f, 0.0f, 0.0f};

    EXPECT_VEC3_EQ(v1 + v2, {7.0f, -3.0f, 2.0f});

    static_assert(v1 + v2 == vglx::Vector3 {7.0f, -3.0f, 2.0f});
}

TEST(Vector3, AdditionNegativeValues) {
    constexpr auto v1 = vglx::Vector3 {-1.0f, -2.0f, -3.0f};
    constexpr auto v2 = vglx::Vector3 {-4.0f, -5.0f, -6.0f};

    EXPECT_VEC3_EQ(v1 + v2, {-5.0f, -7.0f, -9.0f});

    static_assert(v1 + v2 == vglx::Vector3 {-5.0f, -7.0f, -9.0f});
}

TEST(Vector3, AdditionAssignment) {
    auto v1 = vglx::Vector3 {1.0f, 2.0f, 3.0f};
    v1 += vglx::Vector3 {4.0f, 5.0f, 6.0f};

    EXPECT_VEC3_EQ(v1, {5.0f, 7.0f, 9.0f});

    // Compile-time check
    constexpr auto v2 = []() {
        auto v = vglx::Vector3 {1.0f, 2.0f, 3.0f};
        v += vglx::Vector3 {4.0f, 5.0f, 6.0f};
        return v;
    }();

    static_assert(v2 == vglx::Vector3 {5.0f, 7.0f, 9.0f});
}

#pragma endregion

#pragma region Subtraction

TEST(Vector3, SubtractionBasic) {
    constexpr auto v1 = vglx::Vector3 {5.0f, 6.0f, 7.0f};
    constexpr auto v2 = vglx::Vector3 {3.0f, 2.0f, 1.0f};

    EXPECT_VEC3_EQ(v1 - v2, {2.0f, 4.0f, 6.0f});

    static_assert(v1 - v2 == vglx::Vector3 {2.0f, 4.0f, 6.0f});
}

TEST(Vector3, SubtractionFromSelf) {
    constexpr auto v = vglx::Vector3 {9.0f, 8.0f, 7.0f};

    EXPECT_VEC3_EQ(v - v, {0.0f, 0.0f, 0.0f});

    static_assert(v - v == vglx::Vector3 {0.0f, 0.0f, 0.0f});
}

TEST(Vector3, SubtractionFromZeroVector) {
    constexpr auto v1 = vglx::Vector3 {0.0f, 0.0f, 0.0f};
    constexpr auto v2 = vglx::Vector3 {2.0f, 4.0f, 6.0f};

    EXPECT_VEC3_EQ(v1 - v2, {-2.0f, -4.0f, -6.0f});

    static_assert(v1 - v2 == vglx::Vector3 {-2.0f, -4.0f, -6.0f});
}

TEST(Vector3, SubtractionAssignment) {
    auto v1 = vglx::Vector3 {1.0f, 2.0f, 3.0f};
    v1 -= vglx::Vector3 {4.0f, 5.0f, 6.0f};

    EXPECT_VEC3_EQ(v1, {-3.0f, -3.0f, -3.0f});

    // Compile-time check
    constexpr auto v2 = []() {
        auto v = vglx::Vector3 {1.0f, 2.0f, 3.0f};
        v -= vglx::Vector3 {4.0f, 5.0f, 6.0f};
        return v;
    }();

    static_assert(v2 == vglx::Vector3 {-3.0f, -3.0f, -3.0f});
}

#pragma endregion

#pragma region Multiplication

TEST(Vector3, ScalarMultiplicationPositiveScalar) {
    constexpr auto v = vglx::Vector3 {1.0f, 2.0f, 3.0f};

    EXPECT_VEC3_EQ(v * 2.0f, {2.0f, 4.0f, 6.0f});
    EXPECT_VEC3_EQ(2.0f * v, {2.0f, 4.0f, 6.0f});

    static_assert(v * 2.0f == vglx::Vector3 {2.0f, 4.0f, 6.0f});
    static_assert(2.0f * v == vglx::Vector3 {2.0f, 4.0f, 6.0f});
}

TEST(Vector3, ScalarMultiplicationNegativeScalar) {
    constexpr auto v = vglx::Vector3 {1.0f, 2.0f, 3.0f};

    EXPECT_VEC3_EQ(v * -1.0f, {-1.0f, -2.0f, -3.0f});
    EXPECT_VEC3_EQ(-1.0f * v, {-1.0f, -2.0f, -3.0f});

    static_assert(v * -1.0f == vglx::Vector3 {-1.0f, -2.0f, -3.0f});
    static_assert(-1.0f * v == vglx::Vector3 {-1.0f, -2.0f, -3.0f});
}

TEST(Vector3, ScalarMultiplicationZeroScalar) {
    constexpr auto v = vglx::Vector3 {1.0f, 2.0f, 3.0f};

    EXPECT_VEC3_EQ(v * 0.0f, {0.0f, 0.0f, 0.0f});
    EXPECT_VEC3_EQ(0.0f * v, {0.0f, 0.0f, 0.0f});

    static_assert(v * 0.0f == vglx::Vector3 {0.0f, 0.0f, 0.0f});
    static_assert(0.0f * v == vglx::Vector3 {0.0f, 0.0f, 0.0f});
}

TEST(Vector3, ScalarMultiplicationIdentity) {
    constexpr auto v = vglx::Vector3 {1.0f, 2.0f, 3.0f};

    EXPECT_VEC3_EQ(v * 1.0f, {1.0f, 2.0f, 3.0f});
    EXPECT_VEC3_EQ(1.0f * v, {1.0f, 2.0f, 3.0f});

    static_assert(v * 1.0f == vglx::Vector3 {1.0f, 2.0f, 3.0f});
    static_assert(1.0f * v == vglx::Vector3 {1.0f, 2.0f, 3.0f});
}

TEST(Vector3, ScalarMultiplicationAssignment) {
    auto v1 = vglx::Vector3 {1.0f, 2.0f, 3.0f};
    v1 *= 2.0f;

    EXPECT_VEC3_EQ(v1, {2.0f, 4.0f, 6.0f});

    // Compile-time check
    constexpr auto v2 = []() {
        auto v = vglx::Vector3 {1.0f, 2.0f, 3.0f};
        v *= 2.0f;
        return v;
    }();

    static_assert(v2 == vglx::Vector3 {2.0f, 4.0f, 6.0f});
}

TEST(Vector3, VectorMultiplication) {
    constexpr auto v1 = vglx::Vector3 {1.0f, 2.0f, 3.0f};
    constexpr auto v2 = vglx::Vector3 {4.0f, 5.0f, 6.0f};

    EXPECT_VEC3_EQ(v1 * v2, {4.0f, 10.0f, 18.0f});

    static_assert(v1 * v2 == vglx::Vector3 {4.0f, 10.0f, 18.0f});
}

TEST(Vector3, VectorMultiplicationAssignment) {
    auto v1 = vglx::Vector3 {1.0f, 2.0f, 3.0f};
    v1 *= vglx::Vector3 {4.0f, 5.0f, 6.0f};

    EXPECT_VEC3_EQ(v1, {4.0f, 10.0f, 18.0f});

    constexpr auto v2 = []() {
        auto v = vglx::Vector3 {1.0f, 2.0f, 3.0f};
        v *= vglx::Vector3 {4.0f, 5.0f, 6.0f};
        return v;
    }();

    static_assert(v2 == vglx::Vector3 {4.0f, 10.0f, 18.0f});
}

#pragma endregion

#pragma region Division

TEST(Vector3, ScalarDivisionPositiveScalar) {
    constexpr auto v = vglx::Vector3 {2.0f, 4.0f, 6.0f};

    EXPECT_VEC3_EQ(v / 2.0f, {1.0f, 2.0f, 3.0f});

    static_assert(v / 2.0f == vglx::Vector3 {1.0f, 2.0f, 3.0f});
}

TEST(Vector3, ScalarDivisionNegativeScalar) {
    constexpr auto v = vglx::Vector3 {-2.0f, -4.0f, -6.0f};

    EXPECT_VEC3_EQ(v / -2.0f, {1.0f, 2.0f, 3.0f});

    static_assert(v / -2.0f == vglx::Vector3 {1.0f, 2.0f, 3.0f});
}

TEST(Vector3, ScalarDivisionZeroScalar) {
    constexpr auto v = vglx::Vector3 {2.0f, -4.0f, 6.0f};
    constexpr auto inf = std::numeric_limits<float>::infinity();

    EXPECT_VEC3_EQ(v / 0.0f, {inf, -inf, inf});
}

TEST(Vector3, ScalarDivisionIdentity) {
    constexpr auto v = vglx::Vector3 {1.0f, 2.0f, 3.0f};

    EXPECT_VEC3_EQ(v / 1.0f, {1.0f, 2.0f, 3.0f});

    static_assert(v / 1.0f == vglx::Vector3 {1.0f, 2.0f, 3.0f});
}

#pragma endregion

#pragma region MinMax

TEST(Vector3, ComponentwiseMinMax) {
    auto v1 = vglx::Vector3 {2, -1, 5};
    auto v2 = vglx::Vector3 {2, -1, 5};

    v1.Min({3, -2, 4});
    EXPECT_VEC3_EQ(v1, {2, -2, 4});

    v2.Max({3, -2, 4});
    EXPECT_VEC3_EQ(v2, {3, -1, 5});

    constexpr auto v3 = []() {
        auto v = vglx::Vector3 {2, -1, 5};
        v.Min({3, -2, 4});
        return v;
    }();
    static_assert(v3 == vglx::Vector3 {2, -2, 4});

    constexpr auto v4 = []() {
        auto v = vglx::Vector3 {2, -1, 5};
        v.Max({3, -2, 4});
        return v;
    }();
    static_assert(v4 == vglx::Vector3 {3, -1, 5});
}

#pragma endregion

#pragma region Normalize

TEST(Vector3, NormalizeBasic) {
    constexpr auto v = vglx::Vector3 {3.0f, 4.0f, 0.0f};

    EXPECT_VEC3_NEAR(vglx::Normalize(v), {0.6f, 0.8f, 0.0f}, 1e-4);

    static_assert(ApproxEqual(vglx::Normalize(v).x, 0.6f));
    static_assert(ApproxEqual(vglx::Normalize(v).y, 0.8f));
    static_assert(ApproxEqual(vglx::Normalize(v).z, 0.0f));
}

TEST(Vector3, NormalizeNormalizedVector) {
    constexpr auto v = vglx::Vector3 {0.6f, 0.8f, 0.0f};

    EXPECT_VEC3_NEAR(vglx::Normalize(v), {0.6f, 0.8f, 0.0f}, 1e-4);

    static_assert(ApproxEqual(vglx::Normalize(v).x, 0.6f));
    static_assert(ApproxEqual(vglx::Normalize(v).y, 0.8f));
    static_assert(ApproxEqual(vglx::Normalize(v).z, 0.0f));
}

TEST(Vector3, NormalizeZeroVector) {
    constexpr auto v = vglx::Vector3 {0.0f, 0.0f, 0.0f};

    EXPECT_VEC3_EQ(vglx::Normalize(v), {0.0f, 0.0f, 0.0f});

    static_assert(vglx::Normalize(v).x == 0.0f);
    static_assert(vglx::Normalize(v).y == 0.0f);
    static_assert(vglx::Normalize(v).z == 0.0f);
}

TEST(Vector3, NormalizeMemberBasic) {
    auto v1 = vglx::Vector3 {3.0f, 4.0f, 0.0f};
    v1.Normalize();

    EXPECT_VEC3_NEAR(v1, {0.6f, 0.8f, 0.0f}, 1e-4);

    // Compile-time check
    constexpr auto v2 = []() {
        auto v = vglx::Vector3 {3.0f, 4.0f, 0.0f};
        v.Normalize();
        return v;
    }();

    static_assert(ApproxEqual(v2.x, 0.6f));
    static_assert(ApproxEqual(v2.y, 0.8f));
    static_assert(ApproxEqual(v2.z, 0.0f));
}

TEST(Vector3, NormalizeMemberNormalizedVector) {
    auto v1 = vglx::Vector3 {0.6f, 0.8f, 0.0f};
    v1.Normalize();

    EXPECT_VEC3_NEAR(v1, {0.6f, 0.8f, 0.0f}, 1e-4);

    // Compile-time check
    constexpr auto v2 = []() {
        auto v = vglx::Vector3 {0.6f, 0.8f, 0.0f};
        v.Normalize();
        return v;
    }();

    static_assert(ApproxEqual(v2.x, 0.6f));
    static_assert(ApproxEqual(v2.y, 0.8f));
    static_assert(ApproxEqual(v2.z, 0.0f));
}

TEST(Vector3, NormalizedMemberZeroVector) {
    auto v = vglx::Vector3 {0.0f, 0.0f, 0.0f};
    v.Normalize();

    EXPECT_VEC3_EQ(v, {0.0f, 0.0f, 0.0f});
}

#pragma endregion

#pragma region Equality Operator

TEST(Vector3, EqualityOperator) {
    constexpr auto v1 = vglx::Vector3 {1.0f, 2.0f, 3.0f};
    constexpr auto v2 = vglx::Vector3 {1.0f, 2.0f, 3.0f};
    constexpr auto v3 = vglx::Vector3 {4.0f, 5.0f, 6.0f};

    EXPECT_TRUE(v1 == v2);
    EXPECT_FALSE(v1 == v3);

    static_assert(v1 == v2);
    static_assert(v1 != v3);
}

TEST(Vector3, InequalityOperator) {
    constexpr auto v1 = vglx::Vector3 {1.0f, 2.0f, 3.0f};
    constexpr auto v2 = vglx::Vector3 {1.0f, 2.0f, 3.0f};
    constexpr auto v3 = vglx::Vector3 {4.0f, 5.0f, 6.0f};

    EXPECT_FALSE(v1 != v2);
    EXPECT_TRUE(v1 != v3);

    static_assert(v1 == v2);
    static_assert(v1 != v3);
}

#pragma endregion

#pragma region Lerp

TEST(Vector3, Lerp) {
    constexpr auto v1 = vglx::Vector3 {0.0f, 0.0f, 0.0f};
    constexpr auto v2 = vglx::Vector3 {1.0f, 1.0f, 1.0f};

    EXPECT_VEC3_EQ(vglx::Lerp(v1, v2, 0.5f), {0.5f, 0.5f, 0.5f});

    static_assert(vglx::Lerp(v1, v2, 0.5f) == vglx::Vector3 {0.5f, 0.5f, 0.5f});
}

TEST(Vector3, LerpZeroFactor) {
    constexpr auto v1 = vglx::Vector3 {1.0f, 2.0f, 3.0f};
    constexpr auto v2 = vglx::Vector3 {4.0f, 5.0f, 6.0f};

    EXPECT_VEC3_EQ(vglx::Lerp(v1, v2, 0.0f), {1.0f, 2.0f, 3.0f});

    static_assert(vglx::Lerp(v1, v2, 0.0f) == vglx::Vector3 {1.0f, 2.0f, 3.0f});
}

TEST(Vector3, LerpOneFactor) {
    constexpr auto v1 = vglx::Vector3 {1.0f, 2.0f, 3.0f};
    constexpr auto v2 = vglx::Vector3 {4.0f, 5.0f, 6.0f};

    EXPECT_VEC3_EQ(vglx::Lerp(v1, v2, 1.0f), {4.0f, 5.0f, 6.0f});

    static_assert(vglx::Lerp(v1, v2, 1.0f) == vglx::Vector3 {4.0f, 5.0f, 6.0f});
}

#pragma endregion