/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>

#include <vglx/geometries/buffer_attribute.hpp>
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

TEST_F(PlaneGeometryTest, ConstructorInitializesVertexCount) {
    // 9 vertices
    EXPECT_EQ(plane_.VertexCount(), 9);
}

TEST_F(PlaneGeometryTest, ConstructorInitializesIndexData) {
    const auto& index = plane_.GetIndexData();

    // 6 indices (2 triangles per sub-plane), 4 sub-planes
    EXPECT_EQ(index.size(), 6 * 4);
}

TEST_F(PlaneGeometryTest, ConstructorInitializesName) {
    EXPECT_EQ(plane_.Name(), "plane geometry");
}

#pragma endregion

#pragma region Attributes

TEST_F(PlaneGeometryTest, AttributesConfiguredCorrectly) {
    using vglx::BufferAttribute;

    const auto positions = plane_.GetAttribute(BufferAttribute::kPosition);
    ASSERT_NE(positions, nullptr);
    EXPECT_EQ(positions->format, BufferAttribute::Format::Float32x3);

    const auto normals = plane_.GetAttribute(BufferAttribute::kNormal);
    ASSERT_NE(normals, nullptr);
    EXPECT_EQ(normals->format, BufferAttribute::Format::Float32x3);

    const auto uvs = plane_.GetAttribute(BufferAttribute::kTexCoord);
    ASSERT_NE(uvs, nullptr);
    EXPECT_EQ(uvs->format, BufferAttribute::Format::Float32x2);
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
