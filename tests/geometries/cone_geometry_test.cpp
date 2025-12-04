/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>

#include <vglx/geometries/cone_geometry.hpp>

#pragma region Fixtures

class ConeGeometryTest : public ::testing::Test {
protected:
    vglx::ConeGeometry cone_ {{
        .radius = 1.0f,
        .height = 1.0f,
        .radial_segments = 6,
        .height_segments = 1,
        .open_ended = false
    }};
};

#pragma endregion

#pragma region Constructor

TEST_F(ConeGeometryTest, ConstructorInitializesVertexData) {
    const auto verts_size = cone_.VertexData().size();

    // 14 vertices for the torso
    // 13 vertices for the top cap
    // 8 values per vertex, 27 vertices
    EXPECT_EQ(verts_size, 8 * 27);
}

TEST_F(ConeGeometryTest, ConstructorInitializesIndexData) {
    const auto index_size = cone_.IndexData().size();

    // 6 indices per face, 6 faces for the torso (36 total)
    // 3 indices per face, 6 faces for the bottom cap (18 total)
    EXPECT_EQ(index_size, 54);
}

TEST_F(ConeGeometryTest, ConstructorInitializesName) {
    EXPECT_EQ(cone_.Name(), "cone geometry");
}

#pragma endregion

TEST_F(ConeGeometryTest, AttributesConfiguredCorrectly) {
    using enum vglx::Geometry::VertexAttributeType;

    const auto& attrs = cone_.Attributes();

    EXPECT_EQ(attrs[0].type, Position);
    EXPECT_EQ(attrs[0].item_size, 3u);
    EXPECT_EQ(attrs[1].type, Normal);
    EXPECT_EQ(attrs[1].item_size, 3u);
    EXPECT_EQ(attrs[2].type, UV);
    EXPECT_EQ(attrs[2].item_size, 2u);
}

#pragma endregion

#pragma region Assertions

TEST(ConeGeometry, DeathWhenParamsAreInvalid) {
    EXPECT_DEATH({
        vglx::ConeGeometry({.height = 0.0f});
    }, ".*params.height > 0");

    EXPECT_DEATH({
        vglx::ConeGeometry({.radial_segments = 0});
    }, ".*params.radial_segments > 0");

    EXPECT_DEATH({
        vglx::ConeGeometry({.height_segments = 0});
    }, ".*params.height_segments > 0");
}

#pragma endregion