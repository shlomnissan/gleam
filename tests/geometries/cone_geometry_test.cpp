/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>

#include <vglx/geometries/buffer_attribute.hpp>
#include <vglx/primitives/cone_geometry.hpp>

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

TEST_F(ConeGeometryTest, ConstructorInitializesVertexCount) {
    // 14 vertices for the torso
    // 13 vertices for the top cap
    // 27 vertices
    EXPECT_EQ(cone_.VertexCount(), 27);
}

TEST_F(ConeGeometryTest, ConstructorInitializesIndexData) {
    const auto index_size = cone_.GetIndexData().size();

    // 6 indices per face, 6 faces for the torso (36 total)
    // 3 indices per face, 6 faces for the bottom cap (18 total)
    EXPECT_EQ(index_size, 54);
}

TEST_F(ConeGeometryTest, ConstructorInitializesName) {
    EXPECT_EQ(cone_.Name(), "cone geometry");
}

#pragma endregion

#pragma region Attributes

TEST_F(ConeGeometryTest, AttributesConfiguredCorrectly) {
    using vglx::BufferAttribute;

    const auto positions = cone_.GetAttribute(BufferAttribute::kPosition);
    ASSERT_NE(positions, nullptr);
    EXPECT_EQ(positions->format, BufferAttribute::Format::Float32x3);

    const auto normals = cone_.GetAttribute(BufferAttribute::kNormal);
    ASSERT_NE(normals, nullptr);
    EXPECT_EQ(normals->format, BufferAttribute::Format::Float32x3);

    const auto uvs = cone_.GetAttribute(BufferAttribute::kTexCoord);
    ASSERT_NE(uvs, nullptr);
    EXPECT_EQ(uvs->format, BufferAttribute::Format::Float32x2);
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
