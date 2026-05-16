/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>

#include <vglx/primitives/torus_knot_geometry.hpp>

#pragma region Fixtures

class TorusKnotGeometryTest : public ::testing::Test {
protected:
    vglx::TorusKnotGeometry torus_knot_ {{
        .tubular_segments = 3,
        .radial_segments = 3,
    }};
};

#pragma endregion

#pragma region Constructor

TEST_F(TorusKnotGeometryTest, ConstructorInitializesVertexData) {
    const auto& verts = torus_knot_.VertexData();

    // 8 values per vertex, (3+1) * (3+1) = 16 vertices
    EXPECT_EQ(verts.size(), 8 * 16);
}

TEST_F(TorusKnotGeometryTest, ConstructorInitializesIndexData) {
    const auto& index = torus_knot_.IndexData();

    // 6 indices per quad, 3 * 3 quads
    EXPECT_EQ(index.size(), 3 * 3 * 6);
}

TEST_F(TorusKnotGeometryTest, ConstructorInitializesName) {
    EXPECT_EQ(torus_knot_.Name(), "torus knot geometry");
}

#pragma endregion

#pragma region Attributes

TEST_F(TorusKnotGeometryTest, AttributesConfiguredCorrectly) {
    using enum vglx::Geometry::VertexAttributeType;

    const auto& attrs = torus_knot_.Attributes();

    EXPECT_EQ(attrs[0].type, Position);
    EXPECT_EQ(attrs[0].item_size, 3u);
    EXPECT_EQ(attrs[1].type, Normal);
    EXPECT_EQ(attrs[1].item_size, 3u);
    EXPECT_EQ(attrs[2].type, UV);
    EXPECT_EQ(attrs[2].item_size, 2u);
}

#pragma endregion

#pragma region Assertions

TEST(TorusKnotGeometry, DeathWhenParamsAreInvalid) {
    EXPECT_DEATH({
        vglx::TorusKnotGeometry({.radius = 0.0f});
    }, ".*params.radius > 0.0f");

    EXPECT_DEATH({
        vglx::TorusKnotGeometry({.tube = 0.0f});
    }, ".*params.tube > 0.0f");

    EXPECT_DEATH({
        vglx::TorusKnotGeometry({.tubular_segments = 2});
    }, ".*params.tubular_segments >= 3");

    EXPECT_DEATH({
        vglx::TorusKnotGeometry({.radial_segments = 2});
    }, ".*params.radial_segments >= 3");

    EXPECT_DEATH({
        vglx::TorusKnotGeometry({.p = 0});
    }, ".*params.p >= 1");

    EXPECT_DEATH({
        vglx::TorusKnotGeometry({.q = 0});
    }, ".*params.q >= 1");
}

#pragma endregion
