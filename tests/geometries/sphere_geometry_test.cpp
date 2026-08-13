/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>

#include <vglx/geometries/buffer_attribute.hpp>
#include <vglx/primitives/sphere_geometry.hpp>

#pragma region Fixtures

class SphereGeometryTest : public ::testing::Test {
protected:
    vglx::SphereGeometry sphere_ {{
        .width_segments = 3,
        .height_segments = 2,
    }};
};

#pragma endregion

#pragma region Constructor

TEST_F(SphereGeometryTest, ConstructorInitializesVertexCount) {
    // 12 vertices
    EXPECT_EQ(sphere_.VertexCount(), 12);
}

TEST_F(SphereGeometryTest, ConstructorInitializesIndexData) {
    const auto& index = sphere_.GetIndexData();

    // 36 indices (6 quads * 2 triangles)
    EXPECT_EQ(index.size(), 6 * 6);
}

TEST_F(SphereGeometryTest, ConstructorInitializesName) {
    EXPECT_EQ(sphere_.Name(), "sphere geometry");
}

#pragma endregion

#pragma region Attributes

TEST_F(SphereGeometryTest, AttributesConfiguredCorrectly) {
    using vglx::BufferAttribute;

    const auto positions = sphere_.GetAttribute(BufferAttribute::kPosition);
    ASSERT_NE(positions, nullptr);
    EXPECT_EQ(positions->format, BufferAttribute::Format::Float32x3);

    const auto normals = sphere_.GetAttribute(BufferAttribute::kNormal);
    ASSERT_NE(normals, nullptr);
    EXPECT_EQ(normals->format, BufferAttribute::Format::Float32x3);

    const auto uvs = sphere_.GetAttribute(BufferAttribute::kTexCoord);
    ASSERT_NE(uvs, nullptr);
    EXPECT_EQ(uvs->format, BufferAttribute::Format::Float32x2);
}

#pragma endregion

#pragma region Assertions

TEST(SphereGeometry, DeathWhenParamsAreInvalid) {
    EXPECT_DEATH({
        vglx::SphereGeometry({.radius = 0.0f});
    }, ".*params.radius > 0.0f");

    EXPECT_DEATH({
        vglx::SphereGeometry({.width_segments = 2});
    }, ".params.width_segments >= 3");

    EXPECT_DEATH({
        vglx::SphereGeometry({.height_segments = 1});
    }, ".params.height_segments >= 2");
}

#pragma endregion
