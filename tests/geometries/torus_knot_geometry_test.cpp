/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>

#include <vglx/geometries/buffer_attribute.hpp>
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

TEST_F(TorusKnotGeometryTest, ConstructorInitializesVertexCount) {
    // (3+1) * (3+1) = 16 vertices
    EXPECT_EQ(torus_knot_.VertexCount(), 16);
}

TEST_F(TorusKnotGeometryTest, ConstructorInitializesIndexData) {
    const auto& index = torus_knot_.GetIndexData();

    // 6 indices per quad, 3 * 3 quads
    EXPECT_EQ(index.size(), 3 * 3 * 6);
}

TEST_F(TorusKnotGeometryTest, ConstructorInitializesName) {
    EXPECT_EQ(torus_knot_.Name(), "torus knot geometry");
}

#pragma endregion

#pragma region Attributes

TEST_F(TorusKnotGeometryTest, AttributesConfiguredCorrectly) {
    using vglx::BufferAttribute;

    const auto positions = torus_knot_.GetAttribute(BufferAttribute::kPosition);
    ASSERT_NE(positions, nullptr);
    EXPECT_EQ(positions->format, BufferAttribute::Format::Float32x3);

    const auto normals = torus_knot_.GetAttribute(BufferAttribute::kNormal);
    ASSERT_NE(normals, nullptr);
    EXPECT_EQ(normals->format, BufferAttribute::Format::Float32x3);

    const auto uvs = torus_knot_.GetAttribute(BufferAttribute::kTexCoord);
    ASSERT_NE(uvs, nullptr);
    EXPECT_EQ(uvs->format, BufferAttribute::Format::Float32x2);
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
