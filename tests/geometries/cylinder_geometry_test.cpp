/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>

#include <vglx/geometries/buffer_attribute.hpp>
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

TEST_F(CylinderGeometryTest, ConstructorInitializesVertexCount) {
    // 14 vertices for the torso
    // 26 vertices for top and bottom cap
    // 40 vertices
    EXPECT_EQ(cylinder_.VertexCount(), 40);
}

TEST_F(CylinderGeometryTest, ConstructorInitializesIndexData) {
    const auto index_size = cylinder_.GetIndexData().size();

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
    using vglx::BufferAttribute;

    const auto positions = cylinder_.GetAttribute(BufferAttribute::kPosition);
    ASSERT_NE(positions, nullptr);
    EXPECT_EQ(positions->format, BufferAttribute::Format::Float32x3);

    const auto normals = cylinder_.GetAttribute(BufferAttribute::kNormal);
    ASSERT_NE(normals, nullptr);
    EXPECT_EQ(normals->format, BufferAttribute::Format::Float32x3);

    const auto uvs = cylinder_.GetAttribute(BufferAttribute::kTexCoord);
    ASSERT_NE(uvs, nullptr);
    EXPECT_EQ(uvs->format, BufferAttribute::Format::Float32x2);
}

#pragma endregion

#pragma region Assertions

TEST(CylinderGeometry, DeathWhenParamsAreInvalid) {
#ifdef NDEBUG
    GTEST_SKIP() << "VGLX_ASSERT is disabled in release builds";
#endif

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
