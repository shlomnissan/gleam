/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>
#include <test_helpers.hpp>

#include <vglx/math/matrix4.hpp>
#include <vglx/math/quaternion.hpp>
#include <vglx/math/vector3.hpp>
#include <vglx/math/utilities.hpp>

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

TEST(Quaternion, DefaultConstructorIsIdentity) {
    constexpr auto q = vglx::Quaternion {};

    EXPECT_FLOAT_EQ(q.x, 0.0f);
    EXPECT_FLOAT_EQ(q.y, 0.0f);
    EXPECT_FLOAT_EQ(q.z, 0.0f);
    EXPECT_FLOAT_EQ(q.w, 1.0f);

    static_assert(q.x == 0.0f);
    static_assert(q.y == 0.0f);
    static_assert(q.z == 0.0f);
    static_assert(q.w == 1.0f);
}

TEST(Quaternion, ConstructorWithComponents) {
    constexpr auto q = vglx::Quaternion {1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_FLOAT_EQ(q.x, 1.0f);
    EXPECT_FLOAT_EQ(q.y, 2.0f);
    EXPECT_FLOAT_EQ(q.z, 3.0f);
    EXPECT_FLOAT_EQ(q.w, 4.0f);

    static_assert(q.x == 1.0f);
    static_assert(q.y == 2.0f);
    static_assert(q.z == 3.0f);
    static_assert(q.w == 4.0f);
}

TEST(Quaternion, ConstructorWithMatrix) {
    constexpr auto in = vglx::Quaternion::FromAxisAngle(vglx::Vector3 {1.0f, 2.0f, 3.0f}, 1.2f);
    constexpr auto out = vglx::Quaternion {in.GetMatrix()};

    // The recovered rotation must produce the same matrix (q and -q are equal).
    EXPECT_MAT4_NEAR(in.GetMatrix(), out.GetMatrix(), 1e-4);

    static_assert(ApproxEqual(in.GetMatrix()[0].x, out.GetMatrix()[0].x));
    static_assert(ApproxEqual(in.GetMatrix()[1].y, out.GetMatrix()[1].y));
    static_assert(ApproxEqual(in.GetMatrix()[2].z, out.GetMatrix()[2].z));
}

#pragma endregion

#pragma region Identity

TEST(Quaternion, IdentityReturnsZeroVectorAndScalarOne) {
    constexpr auto q = vglx::Quaternion::Identity();

    EXPECT_FLOAT_EQ(q.x, 0.0f);
    EXPECT_FLOAT_EQ(q.y, 0.0f);
    EXPECT_FLOAT_EQ(q.z, 0.0f);
    EXPECT_FLOAT_EQ(q.w, 1.0f);

    static_assert(q == vglx::Quaternion {0.0f, 0.0f, 0.0f, 1.0f});
}

#pragma endregion

#pragma region From Axis Angle

TEST(Quaternion, FromAxisAngleBasic) {
    constexpr auto q = vglx::Quaternion::FromAxisAngle(vglx::Vector3::UnitY(), vglx::math::pi_over_2);

    const auto s = vglx::math::Sin(vglx::math::pi_over_4);
    const auto c = vglx::math::Cos(vglx::math::pi_over_4);

    EXPECT_NEAR(q.x, 0.0f, 1e-4);
    EXPECT_NEAR(q.y, s, 1e-4);
    EXPECT_NEAR(q.z, 0.0f, 1e-4);
    EXPECT_NEAR(q.w, c, 1e-4);

    static_assert(ApproxEqual(q.y, vglx::math::Sin(vglx::math::pi_over_4)));
    static_assert(ApproxEqual(q.w, vglx::math::Cos(vglx::math::pi_over_4)));
}

TEST(Quaternion, FromAxisAngleNormalizesAxis) {
    constexpr auto a = vglx::Quaternion::FromAxisAngle(vglx::Vector3 {0.0f, 5.0f, 0.0f}, 1.0f);
    constexpr auto b = vglx::Quaternion::FromAxisAngle(vglx::Vector3::UnitY(), 1.0f);

    EXPECT_NEAR(a.x, b.x, 1e-4);
    EXPECT_NEAR(a.y, b.y, 1e-4);
    EXPECT_NEAR(a.z, b.z, 1e-4);
    EXPECT_NEAR(a.w, b.w, 1e-4);

    static_assert(ApproxEqual(a.y, b.y));
}

#pragma endregion

#pragma region From Euler

TEST(Quaternion, FromEulerMatchesEulerMatrix) {
    constexpr auto euler = vglx::Euler {0.4f, -1.1f, 2.3f};
    constexpr auto q = vglx::Quaternion::FromEuler(euler);

    EXPECT_MAT4_NEAR(q.GetMatrix(), euler.GetMatrix(), 1e-4);
}

TEST(Quaternion, FromEulerSingleAxisMatchesAxisAngle) {
    constexpr auto pitch = vglx::Quaternion::FromEuler({0.7f, 0.0f, 0.0f});
    constexpr auto yaw = vglx::Quaternion::FromEuler({0.0f, 0.7f, 0.0f});
    constexpr auto roll = vglx::Quaternion::FromEuler({0.0f, 0.0f, 0.7f});

    EXPECT_MAT4_NEAR(
        pitch.GetMatrix(),
        vglx::Quaternion::FromAxisAngle(vglx::Vector3::UnitX(), 0.7f).GetMatrix(),
        1e-4
    );

    EXPECT_MAT4_NEAR(
        yaw.GetMatrix(),
        vglx::Quaternion::FromAxisAngle(vglx::Vector3::UnitY(), 0.7f).GetMatrix(),
        1e-4
    );

    EXPECT_MAT4_NEAR(
        roll.GetMatrix(),
        vglx::Quaternion::FromAxisAngle(vglx::Vector3::UnitZ(), 0.7f).GetMatrix(),
        1e-4
    );
}

TEST(Quaternion, FromEulerIdentityForZeroAngles) {
    constexpr auto q = vglx::Quaternion::FromEuler({});

    EXPECT_NEAR(q.x, 0.0f, 1e-6);
    EXPECT_NEAR(q.y, 0.0f, 1e-6);
    EXPECT_NEAR(q.z, 0.0f, 1e-6);
    EXPECT_NEAR(q.w, 1.0f, 1e-6);
}

#pragma endregion

#pragma region Get Matrix

TEST(Quaternion, GetMatrixMatchesAxisAngleRotation) {
    constexpr auto axis = vglx::Vector3 {1.0f, 2.0f, 3.0f};
    constexpr auto angle = 1.0f;

    constexpr auto q = vglx::Quaternion::FromAxisAngle(axis, angle);
    constexpr auto m = q.GetMatrix();
    constexpr auto expected = Rotate(angle, axis);

    EXPECT_MAT4_NEAR(m, expected, 1e-4);

    static_assert(ApproxEqual(m[0].x, expected[0].x));
    static_assert(ApproxEqual(m[1].y, expected[1].y));
    static_assert(ApproxEqual(m[2].z, expected[2].z));
}

TEST(Quaternion, IdentityGetMatrixIsIdentity) {
    constexpr auto m = vglx::Quaternion::Identity().GetMatrix();

    EXPECT_MAT4_NEAR(m, vglx::Matrix4::Identity(), 1e-4);

    static_assert(m == vglx::Matrix4::Identity());
}

#pragma endregion

#pragma region Multiplication

TEST(Quaternion, CompositionMatchesMatrixProduct) {
    constexpr auto a = vglx::Quaternion::FromAxisAngle(vglx::Vector3::UnitY(), 0.7f);
    constexpr auto b = vglx::Quaternion::FromAxisAngle(vglx::Vector3::UnitX(), 1.1f);

    constexpr auto product = a * b;

    EXPECT_MAT4_NEAR(product.GetMatrix(), a.GetMatrix() * b.GetMatrix(), 1e-4);

    static_assert(ApproxEqual(product.GetMatrix()[0].x, (a.GetMatrix() * b.GetMatrix())[0].x));
}

TEST(Quaternion, IdentityIsMultiplicativeIdentity) {
    constexpr auto q = vglx::Quaternion::FromAxisAngle(vglx::Vector3::UnitZ(), 0.9f);
    constexpr auto r = q * vglx::Quaternion::Identity();

    EXPECT_NEAR(r.x, q.x, 1e-4);
    EXPECT_NEAR(r.y, q.y, 1e-4);
    EXPECT_NEAR(r.z, q.z, 1e-4);
    EXPECT_NEAR(r.w, q.w, 1e-4);

    static_assert(ApproxEqual(r.z, q.z));
    static_assert(ApproxEqual(r.w, q.w));
}

#pragma endregion

#pragma region Rotate Vector

TEST(Quaternion, RotateVectorMatchesMatrix) {
    constexpr auto axis = vglx::Vector3 {1.0f, 2.0f, 3.0f};
    constexpr auto angle = 1.0f;

    constexpr auto q = vglx::Quaternion::FromAxisAngle(axis, angle);
    constexpr auto v = vglx::Vector3 {0.4f, -0.7f, 1.3f};

    constexpr auto rotated = q * v;
    constexpr auto expected = Rotate(angle, axis) * v;

    EXPECT_VEC3_NEAR(rotated, expected, 1e-4);

    static_assert(ApproxEqual(rotated.x, expected.x));
    static_assert(ApproxEqual(rotated.y, expected.y));
    static_assert(ApproxEqual(rotated.z, expected.z));
}

TEST(Quaternion, RotateByIdentityReturnsSameVector) {
    constexpr auto v = vglx::Vector3 {1.0f, 2.0f, 3.0f};
    constexpr auto rotated = vglx::Quaternion::Identity() * v;

    EXPECT_VEC3_NEAR(rotated, v, 1e-4);

    static_assert(ApproxEqual(rotated.x, v.x));
    static_assert(ApproxEqual(rotated.y, v.y));
    static_assert(ApproxEqual(rotated.z, v.z));
}

#pragma endregion

#pragma region Length

TEST(Quaternion, LengthOfUnitQuaternionIsOne) {
    constexpr auto q = vglx::Quaternion::FromAxisAngle(vglx::Vector3 {1.0f, 2.0f, 3.0f}, 1.0f);

    EXPECT_NEAR(q.Length(), 1.0f, 1e-4);

    static_assert(ApproxEqual(q.Length(), 1.0f));
}

TEST(Quaternion, LengthSquared) {
    constexpr auto q = vglx::Quaternion {1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_FLOAT_EQ(q.LengthSquared(), 30.0f);

    static_assert(q.LengthSquared() == 30.0f);
}

TEST(Quaternion, NormalizeMakesUnitLength) {
    auto q = vglx::Quaternion {1.0f, 2.0f, 3.0f, 4.0f};
    q.Normalize();

    EXPECT_NEAR(q.Length(), 1.0f, 1e-4);
}

TEST(Quaternion, NormalizeFreeFunction) {
    constexpr auto q = Normalize(vglx::Quaternion {1.0f, 2.0f, 3.0f, 4.0f});

    EXPECT_NEAR(q.Length(), 1.0f, 1e-4);

    static_assert(ApproxEqual(q.Length(), 1.0f));
}

#pragma endregion

#pragma region Conjugate and Inverse

TEST(Quaternion, ConjugateNegatesVectorPart) {
    constexpr auto q = vglx::Quaternion {1.0f, 2.0f, 3.0f, 4.0f};
    constexpr auto c = Conjugate(q);

    EXPECT_EQ(c, (vglx::Quaternion {-1.0f, -2.0f, -3.0f, 4.0f}));

    static_assert(c == vglx::Quaternion {-1.0f, -2.0f, -3.0f, 4.0f});
}

TEST(Quaternion, InverseOfUnitEqualsConjugate) {
    constexpr auto q = vglx::Quaternion::FromAxisAngle(vglx::Vector3 {1.0f, 2.0f, 3.0f}, 1.0f);
    constexpr auto inv = Inverse(q);
    constexpr auto conj = Conjugate(q);

    EXPECT_NEAR(inv.x, conj.x, 1e-4);
    EXPECT_NEAR(inv.y, conj.y, 1e-4);
    EXPECT_NEAR(inv.z, conj.z, 1e-4);
    EXPECT_NEAR(inv.w, conj.w, 1e-4);

    static_assert(ApproxEqual(inv.x, conj.x));
    static_assert(ApproxEqual(inv.w, conj.w));
}

TEST(Quaternion, QuaternionTimesInverseIsIdentity) {
    constexpr auto q = vglx::Quaternion::FromAxisAngle(vglx::Vector3 {1.0f, 2.0f, 3.0f}, 1.0f);
    constexpr auto r = q * Inverse(q);

    EXPECT_NEAR(r.x, 0.0f, 1e-4);
    EXPECT_NEAR(r.y, 0.0f, 1e-4);
    EXPECT_NEAR(r.z, 0.0f, 1e-4);
    EXPECT_NEAR(r.w, 1.0f, 1e-4);

    static_assert(ApproxEqual(r.w, 1.0f));
}

#pragma endregion

#pragma region Dot

TEST(Quaternion, DotProduct) {
    constexpr auto a = vglx::Quaternion {1.0f, 2.0f, 3.0f, 4.0f};
    constexpr auto b = vglx::Quaternion {5.0f, 6.0f, 7.0f, 8.0f};

    EXPECT_FLOAT_EQ(Dot(a, b), 70.0f);

    static_assert(Dot(a, b) == 70.0f);
}

#pragma endregion

#pragma region Slerp

TEST(Quaternion, SlerpAtZeroReturnsStart) {
    constexpr auto a = vglx::Quaternion::FromAxisAngle(vglx::Vector3::UnitY(), 0.0f);
    constexpr auto b = vglx::Quaternion::FromAxisAngle(vglx::Vector3::UnitY(), vglx::math::pi_over_2);
    constexpr auto r = Slerp(a, b, 0.0f);

    EXPECT_NEAR(r.y, a.y, 1e-4);
    EXPECT_NEAR(r.w, a.w, 1e-4);

    static_assert(ApproxEqual(r.w, a.w));
}

TEST(Quaternion, SlerpAtOneReturnsEnd) {
    constexpr auto a = vglx::Quaternion::FromAxisAngle(vglx::Vector3::UnitY(), 0.0f);
    constexpr auto b = vglx::Quaternion::FromAxisAngle(vglx::Vector3::UnitY(), vglx::math::pi_over_2);
    constexpr auto r = Slerp(a, b, 1.0f);

    EXPECT_NEAR(r.y, b.y, 1e-4);
    EXPECT_NEAR(r.w, b.w, 1e-4);

    static_assert(ApproxEqual(r.y, b.y));
}

TEST(Quaternion, SlerpMidpointIsHalfRotation) {
    constexpr auto a = vglx::Quaternion::Identity();
    constexpr auto b = vglx::Quaternion::FromAxisAngle(vglx::Vector3::UnitY(), vglx::math::pi_over_2);

    constexpr auto mid = Slerp(a, b, 0.5f);
    constexpr auto expected = vglx::Quaternion::FromAxisAngle(vglx::Vector3::UnitY(), vglx::math::pi_over_4);

    EXPECT_NEAR(mid.y, expected.y, 1e-4);
    EXPECT_NEAR(mid.w, expected.w, 1e-4);

    static_assert(ApproxEqual(mid.y, expected.y));
    static_assert(ApproxEqual(mid.w, expected.w));
}

#pragma endregion

#pragma region Equality Operator

TEST(Quaternion, EqualityOperator) {
    constexpr auto a = vglx::Quaternion {1.0f, 2.0f, 3.0f, 4.0f};
    constexpr auto b = vglx::Quaternion {1.0f, 2.0f, 3.0f, 4.0f};
    constexpr auto c = vglx::Quaternion {4.0f, 3.0f, 2.0f, 1.0f};

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);

    static_assert(a == b);
    static_assert(a != c);
}

TEST(Quaternion, InequalityOperator) {
    constexpr auto a = vglx::Quaternion {1.0f, 2.0f, 3.0f, 4.0f};
    constexpr auto b = vglx::Quaternion {1.0f, 2.0f, 3.0f, 4.0f};
    constexpr auto c = vglx::Quaternion {4.0f, 3.0f, 2.0f, 1.0f};

    EXPECT_FALSE(a != b);
    EXPECT_TRUE(a != c);

    static_assert(a == b);
    static_assert(a != c);
}

#pragma endregion
