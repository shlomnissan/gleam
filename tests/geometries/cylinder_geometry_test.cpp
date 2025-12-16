/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>

#include <vglx/primitives/cylinder_geometry.hpp>

#pragma region Fixtures

class CylinderGeometryTest : public ::testing::Test {
protected:
    vglx::CylinderGeometry cylinder_ {{
        .radius_top = 1.0f,
        .radius_bottom = 1.0f,
        .height = 1.0f,
        .radial_segments = 6,
        .height_segments = 1,
        .open_ended = false
    }};
};

#pragma endregion

#pragma region Constructor

TEST_F(CylinderGeometryTest, ConstructorInitializesVertexData) {
    const auto verts_size = cylinder_.VertexData().size();

    // 14 vertices for the torso
    // 26 vertices for top and bottom cap
    // 8 values per vertex, 40 vertices
    EXPECT_EQ(verts_size, 8 * 40);
}

TEST_F(CylinderGeometryTest, ConstructorInitializesIndexData) {
    const auto index_size = cylinder_.IndexData().size();

    // 6 indices per face, 6 faces for the torso (36 total)
    // 3 indices per face, 6 faces for top and bottom cap (36 total)
    EXPECT_EQ(index_size, 72);
}

TEST_F(CylinderGeometryTest, ConstructorInitializesName) {
    EXPECT_EQ(cylinder_.Name(), "cylinder geometry");
}

#pragma endregion

#pragma region Attributes

TEST_F(CylinderGeometryTest, AttributesConfiguredCorrectly) {
    using enum vglx::Geometry::VertexAttributeType;

    const auto& attrs = cylinder_.Attributes();

    EXPECT_EQ(attrs[0].type, Position);
    EXPECT_EQ(attrs[0].item_size, 3u);
    EXPECT_EQ(attrs[1].type, Normal);
    EXPECT_EQ(attrs[1].item_size, 3u);
    EXPECT_EQ(attrs[2].type, UV);
    EXPECT_EQ(attrs[2].item_size, 2u);
}

#pragma endregion

#pragma region Assertions

TEST(CylinderGeometry, DeathWhenParamsAreInvalid) {
    EXPECT_DEATH({
        vglx::CylinderGeometry({.height = 0.0f});
    }, ".*params.height > 0");

    EXPECT_DEATH({
        vglx::CylinderGeometry({.radial_segments = 0});
    }, ".*params.radial_segments > 0");

    EXPECT_DEATH({
        vglx::CylinderGeometry({.height_segments = 0});
    }, ".*params.height_segments > 0");
}

#pragma endregion