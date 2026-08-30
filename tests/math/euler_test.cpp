/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>
#include <test_helpers.hpp>

#include <vglx/math/euler.hpp>
#include <vglx/math/matrix4.hpp>
#include <vglx/math/vector3.hpp>
#include <vglx/math/utilities.hpp>

#include <cassert>

#pragma region Helpers

constexpr auto Rotate(float angle, const vglx::Vector3& v) -> vglx::Matrix4 {
    const auto a = angle;
    const auto c = vglx::math::Cos(a);
    const auto s = vglx::math::Sin(a);
    const auto axis = Normalize(v);
    const auto temp = (1.0f - c) * axis;

    return {
        c + temp[0] * axis[0], temp[1] * axis[0] - s * axis[2], temp[2] * axis[0] + s * axis[1], 0.0f,
        temp[0] * axis[1] + s * axis[2], c + temp[1] * axis[1], temp[2] * axis[1] - s * axis[0], 0.0f,
        temp[0] * axis[2] - s * axis[1], temp[1] * axis[2] + s * axis[0], c + temp[2] * axis[2], 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
}

#pragma endregion

#pragma region Constructor

TEST(Euler, ConstructorWithEulerAngles) {
    constexpr auto e = vglx::Euler {0.5f, 0.2f, 0.3f};

    EXPECT_FLOAT_EQ(e.pitch, 0.5f);
    EXPECT_FLOAT_EQ(e.yaw, 0.2f);
    EXPECT_FLOAT_EQ(e.roll, 0.3f);

    static_assert(e.pitch == 0.5f);
    static_assert(e.yaw == 0.2f);
    static_assert(e.roll == 0.3f);
}

TEST(Euler, ConstructorWithMatrix) {
    constexpr auto in = vglx::Euler {0.5f, 0.2f, 0.3f};
    constexpr auto out = vglx::Euler {in.GetMatrix()};

    EXPECT_NEAR(in.pitch, out.pitch, 1e-4);
    EXPECT_NEAR(in.yaw, out.yaw, 1e-4);
    EXPECT_NEAR(in.roll, out.roll, 1e-4);

    static_assert(ApproxEqual(in.pitch, out.pitch));
    static_assert(ApproxEqual(in.yaw, out.yaw));
    static_assert(ApproxEqual(in.roll, out.roll));
}

#pragma endregion

#pragma region Get Matrix

TEST(Euler, GetMatrixBasic) {
    constexpr auto e = vglx::Euler {0.5f, 0.2f, 0.3f};
    constexpr auto m = e.GetMatrix();

    constexpr auto rotation_x = Rotate(e.pitch, vglx::Vector3::UnitX());
    constexpr auto rotation_y = Rotate(e.yaw, vglx::Vector3::UnitY());
    constexpr auto rotation_z = Rotate(e.roll, vglx::Vector3::UnitZ());

    constexpr auto expected = rotation_y * rotation_x * rotation_z;
    EXPECT_MAT4_NEAR(m, expected, 1e-4);

    static_assert(ApproxEqual(m[0].x, expected[0].x));
    static_assert(ApproxEqual(m[0].y, expected[0].y));
    static_assert(ApproxEqual(m[0].z, expected[0].z));
    static_assert(ApproxEqual(m[0].w, expected[0].w));
    static_assert(ApproxEqual(m[1].x, expected[1].x));
    static_assert(ApproxEqual(m[1].y, expected[1].y));
    static_assert(ApproxEqual(m[1].z, expected[1].z));
    static_assert(ApproxEqual(m[1].w, expected[1].w));
    static_assert(ApproxEqual(m[2].x, expected[2].x));
    static_assert(ApproxEqual(m[2].y, expected[2].y));
    static_assert(ApproxEqual(m[2].z, expected[2].z));
    static_assert(ApproxEqual(m[2].w, expected[2].w));
    static_assert(ApproxEqual(m[3].x, expected[3].x));
    static_assert(ApproxEqual(m[3].y, expected[3].y));
    static_assert(ApproxEqual(m[3].z, expected[3].z));
    static_assert(ApproxEqual(m[3].w, expected[3].w));
}

#pragma endregion

#pragma region Edge Cases

TEST(Euler, ConstrucotrWithMatrixGimbalLock) {
    constexpr auto pitch = vglx::math::pi_over_2;
    constexpr auto yaw = 0.0f;
    constexpr auto roll = 0.0f;

    constexpr auto e = vglx::Euler {pitch, yaw, roll};
    constexpr auto m = e.GetMatrix();
    constexpr auto output = vglx::Euler {m};

    // Check if the pitch is correctly identified as 90 degrees
    EXPECT_NEAR(output.pitch, pitch, 1e-4);

    // In gimbal lock, yaw and roll are not uniquely determined
    // We can only check if the sum of yaw and roll is correct
    EXPECT_NEAR(output.yaw + output.roll, yaw + roll, 1e-4);

    static_assert(ApproxEqual(output.pitch, pitch));
    static_assert(ApproxEqual(output.yaw+ output.roll, yaw + roll));
}


#pragma endregion

#pragma region Equality Operator

TEST(Euler, EqualityOperator) {
    constexpr auto e1 = vglx::Euler {0.5f, 0.2f, 0.0f};
    constexpr auto e2 = vglx::Euler {0.5f, 0.2f, 0.0f};
    constexpr auto e3 = vglx::Euler {0.1f, 0.2f, 0.0f};

    EXPECT_TRUE(e1 == e2);
    EXPECT_FALSE(e1 == e3);

    static_assert(e1 == e2);
    static_assert(e1 != e3);
}

TEST(Euler, InequalityOperator) {
    constexpr auto e1 = vglx::Euler {0.5f, 0.2f, 0.0f};
    constexpr auto e2 = vglx::Euler {0.5f, 0.2f, 0.0f};
    constexpr auto e3 = vglx::Euler {0.1f, 0.2f, 0.0f};

    EXPECT_FALSE(e1 != e2);
    EXPECT_TRUE(e1 != e3);

    static_assert(e1 == e2);
    static_assert(e1 != e3);
}

#pragma endregion

#pragma region Empty

TEST(Euler, IsEmptyReturnsTrue) {
    constexpr auto e = vglx::Euler {};

    EXPECT_TRUE(e.IsEmpty());

    static_assert(e.IsEmpty());
}

TEST(Euler, IsEmptyReturnsFalse) {
    constexpr auto e = vglx::Euler {0.5f, 0.2f, 0.0f};

    EXPECT_FALSE(e.IsEmpty());

    static_assert(!e.IsEmpty());
}

#pragma endregion