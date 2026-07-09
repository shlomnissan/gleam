/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>
#include <test_helpers.hpp>

#include <vglx/math/quaternion.hpp>
#include <vglx/math/transform3.hpp>
#include <vglx/math/utilities.hpp>

#pragma region Mutators

TEST(Transform3, SetPosition) {
    auto t1 = vglx::Transform3 {};
    t1.SetPosition({2.0f, 1.0f, 3.0f});

    EXPECT_VEC3_EQ(t1.position, {2.0f, 1.0f, 3.0f});
    EXPECT_MAT4_EQ(t1.Get(), {
        1.0f, 0.0f, 0.0f, 2.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 3.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    });

    constexpr auto t2 = []() {
        auto t = vglx::Transform3 {};
        t.SetPosition({2.0f, 1.0f, 3.0f});
        return t;
    }();

    static_assert(t2.position.x == 2.0f);
    static_assert(t2.position.y == 1.0f);
    static_assert(t2.position.z == 3.0f);
}

TEST(Transform3, SetScale) {
    auto t1 = vglx::Transform3 {};
    t1.SetScale({2.0f, 1.0f, 3.0f});

    EXPECT_VEC3_EQ(t1.scale, {2.0f, 1.0f, 3.0f});
    EXPECT_MAT4_EQ(t1.Get(), {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 3.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    });

    constexpr auto t2 = []() {
        auto t = vglx::Transform3 {};
        t.SetScale({2.0f, 1.0f, 3.0f});
        return t;
    }();

    static_assert(t2.scale.x == 2.0f);
    static_assert(t2.scale.y == 1.0f);
    static_assert(t2.scale.z == 3.0f);
}

TEST(Transform3, SetRotationFromEuler) {
    constexpr auto e = vglx::Euler {0.1f, 0.2f, 0.3f};

    auto t1 = vglx::Transform3 {};
    t1.SetRotation(e);

    // The rotation is stored as a quaternion; Get() reproduces the Euler matrix.
    EXPECT_MAT4_NEAR(t1.Get(), e.GetMatrix(), 1e-4);

    // Euler read-back is well defined away from gimbal lock.
    const auto out = t1.GetEuler();
    EXPECT_NEAR(out.pitch, e.pitch, 1e-4);
    EXPECT_NEAR(out.yaw, e.yaw, 1e-4);
    EXPECT_NEAR(out.roll, e.roll, 1e-4);

    constexpr auto t2 = []() {
        auto t = vglx::Transform3 {};
        t.SetRotation(vglx::Euler {0.1f, 0.2f, 0.3f});
        return t;
    }();

    static_assert(ApproxEqual(t2.GetEuler().pitch, 0.1f));
}

TEST(Transform3, SetRotationFromQuaternion) {
    constexpr auto q = vglx::Quaternion::FromAxisAngle(vglx::Vector3::Y(), vglx::math::pi_over_2);

    auto t1 = vglx::Transform3 {};
    t1.SetRotation(q);

    EXPECT_EQ(t1.rotation, q);
    EXPECT_MAT4_NEAR(t1.Get(), q.GetMatrix(), 1e-4);

    constexpr auto t2 = []() {
        auto t = vglx::Transform3 {};
        t.SetRotation(vglx::Quaternion::FromAxisAngle(vglx::Vector3::Y(), vglx::math::pi_over_2));
        return t;
    }();

    static_assert(t2.rotation == q);
}

TEST(Transform3, MultipleTransformations) {
    constexpr auto rotation = vglx::Euler {
        vglx::math::pi_over_2 + 0.1f,
        vglx::math::pi_over_2 + 0.2f,
        vglx::math::pi_over_2 + 0.3f
    };
    constexpr auto position = vglx::Vector3 {2.0f, 1.0f, 3.0f};
    constexpr auto scale = vglx::Vector3 {2.0f, 1.0f, 3.0f};

    auto t = vglx::Transform3 {};
    t.SetPosition(position);
    t.SetScale(scale);
    t.SetRotation(rotation);

    const auto cos_p = vglx::math::Cos(rotation.pitch);
    const auto sin_p = vglx::math::Sin(rotation.pitch);
    const auto cos_y = vglx::math::Cos(rotation.yaw);
    const auto sin_y = vglx::math::Sin(rotation.yaw);
    const auto cos_r = vglx::math::Cos(rotation.roll);
    const auto sin_r = vglx::math::Sin(rotation.roll);

    EXPECT_MAT4_NEAR(t.Get(), {
        scale.x * (cos_r * cos_y - sin_r * sin_p * sin_y),
        scale.y * (-sin_r * cos_p),
        scale.z * (cos_r * sin_y + sin_r * sin_p * cos_y),
        position.x,

        scale.x * (sin_r * cos_y + cos_r * sin_p * sin_y),
        scale.y * (cos_r * cos_p),
        scale.z * (sin_r * sin_y - cos_r * sin_p * cos_y),
        position.y,

        scale.x * (-cos_p * sin_y),
        scale.y * sin_p,
        scale.z * (cos_p * cos_y),
        position.z,

        0.0f, 0.0f, 0.0f, 1.0f
    }, 1e-4);
}

#pragma endregion

#pragma region Cumulative Transformations

TEST(Transform3, Translate) {
    auto t1 = vglx::Transform3 {};
    t1.Translate({2.0f, 1.0f, 3.0f});
    t1.Translate({1.0f, 1.0f, 0.0f});

    EXPECT_VEC3_EQ(t1.position, {3.0f, 2.0f, 3.0f});
    EXPECT_MAT4_EQ(t1.Get(), {
        1.0f, 0.0f, 0.0f, 3.0f,
        0.0f, 1.0f, 0.0f, 2.0f,
        0.0f, 0.0f, 1.0f, 3.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    });

    constexpr auto t2 = []() {
        auto t = vglx::Transform3 {};
        t.Translate({2.0f, 1.0f, 3.0f});
        t.Translate({1.0f, 1.0f, 0.0f});
        return t;
    }();

    static_assert(t2.position.x == 3.0f);
    static_assert(t2.position.y == 2.0f);
    static_assert(t2.position.z == 3.0f);
}

TEST(Transform3, Scale) {
    auto t1 = vglx::Transform3 {};
    t1.Scale({2.0f, 2.0f, 2.0f});
    t1.Scale({3.0f, 3.0f, 2.0f});

    EXPECT_VEC3_EQ(t1.scale, {6.0f, 6.0f, 4.0f});
    EXPECT_MAT4_EQ(t1.Get(), {
        6.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 6.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 4.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    });

    constexpr auto t2 = []() {
        auto t = vglx::Transform3 {};
            t.Scale({2.0f, 2.0f, 2.0f});
            t.Scale({3.0f, 3.0f, 2.0f});
        return t;
    }();

    static_assert(t2.scale.x == 6.0f);
    static_assert(t2.scale.y == 6.0f);
    static_assert(t2.scale.z == 4.0f);
}

TEST(Transform3, RotateX) {
    auto t = vglx::Transform3 {};
    t.Rotate(vglx::Vector3::X(), vglx::math::pi_over_2);
    t.Rotate(vglx::Vector3::X(), 0.1f);

    constexpr auto c = vglx::math::Cos(vglx::math::pi_over_2 + 0.1f);
    constexpr auto s = vglx::math::Sin(vglx::math::pi_over_2 + 0.1f);

    EXPECT_MAT4_NEAR(t.Get(), {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, c, -s, 0.0f,
        0.0f, s, c, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    }, 1e-4);

    constexpr auto m = []() {
        auto t = vglx::Transform3 {};
        t.Rotate(vglx::Vector3::X(), vglx::math::pi_over_2);
        t.Rotate(vglx::Vector3::X(), 0.1f);
        return t.Get();
    }();

    static_assert(ApproxEqual(m[1].y, c));
    static_assert(ApproxEqual(m[1].z, s));
    static_assert(ApproxEqual(m[2].y, -s));
    static_assert(ApproxEqual(m[2].z, c));
}

TEST(Transform3, RotateY) {
    auto t = vglx::Transform3 {};
    t.Rotate(vglx::Vector3::Y(), vglx::math::pi_over_2);
    t.Rotate(vglx::Vector3::Y(), 0.1f);

    constexpr auto c = vglx::math::Cos(vglx::math::pi_over_2 + 0.1f);
    constexpr auto s = vglx::math::Sin(vglx::math::pi_over_2 + 0.1f);

    EXPECT_MAT4_NEAR(t.Get(), {
        c, 0.0f, s, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        -s, 0.0f, c, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    }, 1e-4);

    constexpr auto m = []() {
        auto t = vglx::Transform3 {};
        t.Rotate(vglx::Vector3::Y(), vglx::math::pi_over_2);
        t.Rotate(vglx::Vector3::Y(), 0.1f);
        return t.Get();
    }();

    static_assert(ApproxEqual(m[0].x, c));
    static_assert(ApproxEqual(m[0].z, -s));
    static_assert(ApproxEqual(m[2].x, s));
    static_assert(ApproxEqual(m[2].z, c));
}

TEST(Transform3, RotateZ) {
    auto t = vglx::Transform3 {};
    t.Rotate(vglx::Vector3::Z(), vglx::math::pi_over_2);
    t.Rotate(vglx::Vector3::Z(), 0.1f);

    constexpr auto c = vglx::math::Cos(vglx::math::pi_over_2 + 0.1f);
    constexpr auto s = vglx::math::Sin(vglx::math::pi_over_2 + 0.1f);

    EXPECT_MAT4_NEAR(t.Get(), {
        c, -s, 0.0f, 0.0f,
        s, c, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    }, 1e-4);

    constexpr auto m = []() {
        auto t = vglx::Transform3 {};
        t.Rotate(vglx::Vector3::Z(), vglx::math::pi_over_2);
        t.Rotate(vglx::Vector3::Z(), 0.1f);
        return t.Get();
    }();

    static_assert(ApproxEqual(m[0].x, c));
    static_assert(ApproxEqual(m[0].y, s));
    static_assert(ApproxEqual(m[1].x, -s));
    static_assert(ApproxEqual(m[1].y, c));
}

TEST(Transform3, RotateArbitraryAxis) {
    constexpr auto axis = vglx::Vector3 {1.0f, 2.0f, 3.0f};
    constexpr auto angle = 1.0f;

    auto t = vglx::Transform3 {};
    t.Rotate(axis, angle);

    EXPECT_MAT4_NEAR(t.Get(), vglx::Quaternion::FromAxisAngle(axis, angle).GetMatrix(), 1e-4);

    constexpr auto m = []() {
        auto t = vglx::Transform3 {};
        t.Rotate(vglx::Vector3 {1.0f, 2.0f, 3.0f}, 1.0f);
        return t.Get();
    }();

    constexpr auto expected = vglx::Quaternion::FromAxisAngle(axis, angle).GetMatrix();
    static_assert(ApproxEqual(m[0].x, expected[0].x));
    static_assert(ApproxEqual(m[1].y, expected[1].y));
    static_assert(ApproxEqual(m[2].z, expected[2].z));
}

#pragma endregion

#pragma region Local-Space Translation

TEST(Transform3, TranslateBeforeRotation) {
    auto t = vglx::Transform3 {};
    t.Translate({0.0f, 0.0f, 1.0f});
    t.Rotate(vglx::Vector3::Y(), vglx::math::pi_over_2);

    EXPECT_MAT4_NEAR(t.Get(), {
         0.0f, 0.0f, 1.0f, 0.0f,
         0.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f, 1.0f,
         0.0f, 0.0f, 0.0f, 1.0f
    }, 1e-4);

    constexpr auto m = []() {
        auto t = vglx::Transform3 {};
        t.Translate({0.0f, 0.0f, 1.0f});
        t.Rotate(vglx::Vector3::Y(), vglx::math::pi_over_2);
        return t.Get();
    }();

    static_assert(ApproxEqual(m[0].x, 0.0f));
    static_assert(ApproxEqual(m[0].y, 0.0f));
    static_assert(ApproxEqual(m[0].z, -1.0f));
    static_assert(ApproxEqual(m[1].x, 0.0f));
    static_assert(ApproxEqual(m[1].y, 1.0f));
    static_assert(ApproxEqual(m[1].z, 0.0f));
    static_assert(ApproxEqual(m[2].x, 1.0f));
    static_assert(ApproxEqual(m[2].y, 0.0f));
    static_assert(ApproxEqual(m[2].z, 0.0f));
    static_assert(ApproxEqual(m[3].x, 0.0f));
    static_assert(ApproxEqual(m[3].y, 0.0f));
    static_assert(ApproxEqual(m[3].z, 1.0f));
}

TEST(Transform3, TranslateAfterRotation) {
    auto t = vglx::Transform3 {};
    t.Rotate(vglx::Vector3::Y(), vglx::math::pi_over_2);
    t.Translate({0.0f, 0.0f, 1.0f});

    EXPECT_MAT4_NEAR(t.Get(), {
         0.0f, 0.0f, 1.0f, 1.0f,
         0.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 1.0f
    }, 1e-4);

    constexpr auto m = []() {
        auto t = vglx::Transform3 {};
        t.Rotate(vglx::Vector3::Y(), vglx::math::pi_over_2);
        t.Translate({0.0f, 0.0f, 1.0f});
        return t.Get();
    }();

    static_assert(ApproxEqual(m[0].x, 0.0f));
    static_assert(ApproxEqual(m[0].y, 0.0f));
    static_assert(ApproxEqual(m[0].z, -1.0f));
    static_assert(ApproxEqual(m[1].x, 0.0f));
    static_assert(ApproxEqual(m[1].y, 1.0f));
    static_assert(ApproxEqual(m[1].z, 0.0f));
    static_assert(ApproxEqual(m[2].x, 1.0f));
    static_assert(ApproxEqual(m[2].y, 0.0f));
    static_assert(ApproxEqual(m[2].z, 0.0f));
    static_assert(ApproxEqual(m[3].x, 1.0f));
    static_assert(ApproxEqual(m[3].y, 0.0f));
    static_assert(ApproxEqual(m[3].z, 0.0f));
}

#pragma endregion
