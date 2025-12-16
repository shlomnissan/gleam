/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>

#include <vglx/primitives/plane_geometry.hpp>

#pragma region Fixtures

class PlaneGeometryTest : public ::testing::Test {
protected:
    vglx::PlaneGeometry plane_ {{
        .width = 1.0f,
        .height = 1.0f,
        .width_segments = 2,
        .height_segments = 2
    }};
};

#pragma endregion

#pragma region Constructor

TEST_F(PlaneGeometryTest, ConstructorInitializesVertexData) {
    const auto& verts = plane_.VertexData();

    // 8 values per vertex, 9 vertices
    EXPECT_EQ(verts.size(), 8 * 9);
}

TEST_F(PlaneGeometryTest, ConstructorInitializesIndexData) {
    const auto& index = plane_.IndexData();

    // 6 indices (2 triangles per sub-plane), 4 sub-planes
    EXPECT_EQ(index.size(), 6 * 4);
}

TEST_F(PlaneGeometryTest, ConstructorInitializesName) {
    EXPECT_EQ(plane_.Name(), "plane geometry");
}

#pragma endregion

#pragma region Attributes

TEST_F(PlaneGeometryTest, AttributesConfiguredCorrectly) {
    using enum vglx::Geometry::VertexAttributeType;

    const auto& attrs = plane_.Attributes();

    EXPECT_EQ(attrs[0].type, Position);
    EXPECT_EQ(attrs[0].item_size, 3u);
    EXPECT_EQ(attrs[1].type, Normal);
    EXPECT_EQ(attrs[1].item_size, 3u);
    EXPECT_EQ(attrs[2].type, UV);
    EXPECT_EQ(attrs[2].item_size, 2u);
}

#pragma endregion

#pragma region Assertions

TEST(PlaneGeometry, DeathWhenParamsAreInvalid) {
    EXPECT_DEATH({
        vglx::PlaneGeometry({.width = 0.0f});
    }, ".*params.width > 0");

    EXPECT_DEATH({
        vglx::PlaneGeometry({.height = 0.0f});
    }, ".*params.height > 0");

    EXPECT_DEATH({
        vglx::PlaneGeometry({.width_segments = 0});
    }, ".*params.width_segments > 0");

    EXPECT_DEATH({
        vglx::PlaneGeometry({ .height_segments = 0});
    }, ".*params.height_segments > 0");
}

#pragma endregion