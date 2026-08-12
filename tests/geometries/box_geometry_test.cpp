/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>

#include <vglx/geometries/buffer_attribute.hpp>
#include <vglx/primitives/box_geometry.hpp>

#pragma region Fixtures

class BoxGeometryTest : public ::testing::Test {
protected:
    vglx::BoxGeometry box_ {{
        .width = 1.0f,
        .height = 1.0f,
        .depth = 1.0f,
        .width_segments = 2,
        .height_segments = 2,
        .depth_segments = 2
    }};
};

#pragma endregion

#pragma region Constructor

TEST_F(BoxGeometryTest, ConstructorInitializesVertexData) {
    // 9 vertices, 6 faces
    EXPECT_EQ(box_.VertexCount(), 9 * 6);
}

TEST_F(BoxGeometryTest, ConstructorInitializesIndexData) {
    const auto& index = box_.GetIndexData();

    // 6 indices (2 triangles per sub-plane), 4 sub-planes, 6 faces
    EXPECT_EQ(index.size(), 6 * 4 * 6);
}

TEST_F(BoxGeometryTest, ConstructorInitializesName) {
    EXPECT_EQ(box_.Name(), "box geometry");
}

#pragma endregion

#pragma region Attributes

TEST_F(BoxGeometryTest, AttributesConfiguredCorrectly) {
    using vglx::BufferAttribute;

    const auto positions = box_.GetAttribute(BufferAttribute::kPosition);
    ASSERT_NE(positions, nullptr);
    EXPECT_EQ(positions->format, BufferAttribute::Format::Float32x3);

    const auto normals = box_.GetAttribute(BufferAttribute::kNormal);
    ASSERT_NE(normals, nullptr);
    EXPECT_EQ(normals->format, BufferAttribute::Format::Float32x3);

    const auto uvs = box_.GetAttribute(BufferAttribute::kTexCoord);
    ASSERT_NE(uvs, nullptr);
    EXPECT_EQ(uvs->format, BufferAttribute::Format::Float32x2);
}

#pragma endregion

#pragma region Assertions

TEST(BoxGeometry, DeathWhenParamsAreInvalid) {
    EXPECT_DEATH({
        vglx::BoxGeometry({.width = 0.0f});
    }, ".*params.width > 0");

    EXPECT_DEATH({
        vglx::BoxGeometry({.height = 0.0f});
    }, ".*params.height > 0");

    EXPECT_DEATH({
        vglx::BoxGeometry({.depth = 0.0f});
    }, ".*params.depth > 0");

    EXPECT_DEATH({
        vglx::BoxGeometry({.width_segments = 0});
    }, ".*params.width_segments > 0");

    EXPECT_DEATH({
        vglx::BoxGeometry({.height_segments = 0});
    }, ".*params.height_segments > 0");

    EXPECT_DEATH({
        vglx::BoxGeometry({.depth_segments = 0});
    }, ".*params.depth_segments > 0");
}

#pragma endregion