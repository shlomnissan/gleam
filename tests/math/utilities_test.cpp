/*
===========================================================================
  GLEAM ENGINE https://gleamengine.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>

#include <cassert>
#include <regex>

#include <gleam/math/utilities.hpp>

using namespace gleam;

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

#pragma region UUID

TEST(MathUtilities, UUIDFormat) {
    static const std::regex e("^[0-9a-f]{8}-[0-9a-f]{4}-[0-5][0-9a-f]{3}-[089ab][0-9a-f]{3}-[0-9a-f]{12}$", std::regex_constants::icase);
    EXPECT_TRUE(std::regex_match(math::GenerateUUID(), e));
}

TEST(MathUtilities, UUIDUniqueness) {
    std::set<std::string> uuids;
    for (auto i = 0; i < 1000; i++) {
        const auto uuid = math::GenerateUUID();
        EXPECT_TRUE(uuids.emplace(uuid).second);
    }
}

#pragma endregion