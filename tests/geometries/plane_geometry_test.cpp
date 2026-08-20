/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>

#include <vglx/geometries/buffer_attribute.hpp>
#include <vglx/math/vector3.hpp>
#include <vglx/primitives/plane_geometry.hpp>

#include <string_view>
#include <vector>

namespace {

auto create_plane(vglx::PlaneGeometry::Orientation orientation) {
    return vglx::PlaneGeometry {{
        .width = 4.0f,
        .height = 6.0f,
        .width_segments = 2,
        .height_segments = 2,
        .orientation = orientation
    }};
}

auto get_vectors(const vglx::Geometry& geometry, std::string_view name) {
    const auto attribute = geometry.GetAttribute(name);
    const auto& data = attribute->GetData();

    auto result = std::vector<vglx::Vector3> {};
    for (auto i = 0u; i < data.size(); i += 3) {
        result.emplace_back(data[i], data[i + 1], data[i + 2]);
    }

    return result;
}

}

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

#pragma region Orientation

TEST(PlaneGeometry, OrientationPreservesTriangleWinding) {
    using enum vglx::PlaneGeometry::Orientation;

    for (const auto orientation : {FaceX, FaceY, FaceZ}) {
        const auto plane = create_plane(orientation);
        const auto positions = get_vectors(plane, vglx::BufferAttribute::kPosition);
        const auto normals = get_vectors(plane, vglx::BufferAttribute::kNormal);
        const auto& index = plane.GetIndexData();

        for (auto i = 0u; i < index.size(); i += 3) {
            SCOPED_TRACE(::testing::Message()
                << "orientation " << static_cast<int>(orientation) << ", triangle " << i / 3);

            const auto& p0 = positions[index[i]];
            const auto& p1 = positions[index[i + 1]];
            const auto& p2 = positions[index[i + 2]];

            // triangles must wind counter-clockwise when viewed from the faced axis
            const auto face = vglx::Cross(p1 - p0, p2 - p0);
            EXPECT_GT(vglx::Dot(face, normals[index[i]]), 0.0f);
        }
    }
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
