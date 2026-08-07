/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>
#include <test_helpers.hpp>

#include <vglx/geometries/buffer_attribute.hpp>
#include <vglx/geometries/geometry2.hpp>

#include <utility>
#include <vector>

using vglx::BufferAttribute;
using vglx::Geometry2;

namespace {

const auto kTrianglePositions = std::vector<float> {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

auto create_positions(std::vector<float> data) {
    return BufferAttribute::Create({
        .name = BufferAttribute::kPosition,
        .format = BufferAttribute::Format::Float32x3,
        .rate = BufferAttribute::Rate::Vertex
    }, std::move(data));
}

auto create_tex_coords(std::vector<float> data) {
    return BufferAttribute::Create({
        .name = BufferAttribute::kTexCoord,
        .format = BufferAttribute::Format::Float32x2,
        .rate = BufferAttribute::Rate::Vertex
    }, std::move(data));
}

}

#pragma region Attributes

TEST(Geometry2, AddAttribute) {
    auto geometry = Geometry2::Create();
    auto positions = create_positions(kTrianglePositions);

    geometry->AddAttribute(positions);

    EXPECT_EQ(geometry->GetAttribute(BufferAttribute::kPosition), positions);
    EXPECT_EQ(geometry->VertexCount(), 3);
    EXPECT_EQ(geometry->GetLayoutVersion(), 1);
}

TEST(Geometry2, AddMultipleAttributesWithMatchingCounts) {
    auto geometry = Geometry2::Create();

    geometry->AddAttribute(create_positions(kTrianglePositions));
    geometry->AddAttribute(create_tex_coords({
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.5f, 1.0f
    }));

    EXPECT_NE(geometry->GetAttribute(BufferAttribute::kPosition), nullptr);
    EXPECT_NE(geometry->GetAttribute(BufferAttribute::kTexCoord), nullptr);
    EXPECT_EQ(geometry->VertexCount(), 3);
    EXPECT_EQ(geometry->GetLayoutVersion(), 2);
}

TEST(Geometry2, RejectsAttributeWithDuplicateName) {
    auto geometry = Geometry2::Create();
    auto first_attribute = create_positions(kTrianglePositions);

    geometry->AddAttribute(first_attribute);
    geometry->AddAttribute(create_positions(kTrianglePositions));

    EXPECT_EQ(geometry->GetAttribute(BufferAttribute::kPosition), first_attribute);
    EXPECT_EQ(geometry->GetLayoutVersion(), 1);
}

TEST(Geometry2, RejectsAttributeWithInstanceRate) {
    auto geometry = Geometry2::Create();

    geometry->AddAttribute(BufferAttribute::Create({
        .name = BufferAttribute::kInstanceColor,
        .format = BufferAttribute::Format::Float32x3,
        .rate = BufferAttribute::Rate::Instance
    }, {1.0f, 0.0f, 0.0f}));

    EXPECT_EQ(geometry->GetAttribute(BufferAttribute::kInstanceColor), nullptr);
    EXPECT_EQ(geometry->GetLayoutVersion(), 0);
}

TEST(Geometry2, RejectsInvalidAttribute) {
    auto geometry = Geometry2::Create();

    geometry->AddAttribute(create_positions({}));

    EXPECT_EQ(geometry->GetAttribute(BufferAttribute::kPosition), nullptr);
    EXPECT_EQ(geometry->GetLayoutVersion(), 0);
}

TEST(Geometry2, RejectsAttributeWithElementCountMismatch) {
    auto geometry = Geometry2::Create();

    geometry->AddAttribute(create_positions(kTrianglePositions));

    // 4 elements for 3 existing vertices
    geometry->AddAttribute(create_tex_coords({
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    }));

    EXPECT_EQ(geometry->GetAttribute(BufferAttribute::kTexCoord), nullptr);
    EXPECT_EQ(geometry->GetLayoutVersion(), 1);
}

#pragma endregion

#pragma region Indices

TEST(Geometry2, SetIndices) {
    auto geometry = Geometry2::Create();

    geometry->SetIndices({0, 1, 2, 2, 3, 0});

    EXPECT_EQ(geometry->GetIndexData().size(), 6);
    EXPECT_EQ(geometry->GetMaxIndex(), 3);
    EXPECT_EQ(geometry->GetIndexVersion(), 1);
}

TEST(Geometry2, SetIndicesWithEmptyList) {
    auto geometry = Geometry2::Create();

    geometry->SetIndices({0, 1, 2});
    geometry->SetIndices({});

    EXPECT_TRUE(geometry->GetIndexData().empty());
    EXPECT_EQ(geometry->GetMaxIndex(), 0);
    EXPECT_EQ(geometry->GetIndexVersion(), 2);
}

#pragma endregion

#pragma region Bounding Volumes

TEST(Geometry2, BoundingBox) {
    auto geometry = Geometry2::Create();
    geometry->AddAttribute(create_positions(kTrianglePositions));

    const auto box = geometry->BoundingBox();

    EXPECT_VEC3_EQ(box.min, {-0.5f, -0.5f, 0.0f});
    EXPECT_VEC3_EQ(box.max, {0.5f, 0.5f, 0.0f});
}

TEST(Geometry2, BoundingSphere) {
    auto geometry = Geometry2::Create();
    geometry->AddAttribute(create_positions({
        -1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f
    }));

    const auto sphere = geometry->BoundingSphere();

    EXPECT_VEC3_EQ(sphere.center, {0.0f, 0.0f, 0.0f});
    EXPECT_NEAR(sphere.radius, 1.0f, 1e-4f);
}

TEST(Geometry2, BoundingBoxWithoutPositionAttribute) {
    auto geometry = Geometry2::Create();

    const auto box = geometry->BoundingBox();
    const auto default_box = vglx::Box3 {};

    EXPECT_VEC3_EQ(box.min, default_box.min);
    EXPECT_VEC3_EQ(box.max, default_box.max);
}

TEST(Geometry2, BoundingSphereWithoutPositionAttribute) {
    auto geometry = Geometry2::Create();

    const auto sphere = geometry->BoundingSphere();

    EXPECT_FLOAT_EQ(sphere.radius, -1.0f);
}

TEST(Geometry2, BoundingBoxInvalidatedWhenPositionDataChanges) {
    auto geometry = Geometry2::Create();
    auto positions = create_positions(kTrianglePositions);
    geometry->AddAttribute(positions);

    const auto before = geometry->BoundingBox();
    EXPECT_VEC3_EQ(before.max, {0.5f, 0.5f, 0.0f});

    positions->SetData({
        -2.0f, -2.0f, 0.0f,
         2.0f, -2.0f, 0.0f,
         0.0f,  2.0f, 0.0f
    });

    const auto after = geometry->BoundingBox();
    EXPECT_VEC3_EQ(after.min, {-2.0f, -2.0f, 0.0f});
    EXPECT_VEC3_EQ(after.max, {2.0f, 2.0f, 0.0f});
}

TEST(Geometry2, BoundingSphereInvalidatedWhenPositionDataChanges) {
    auto geometry = Geometry2::Create();
    auto positions = create_positions({
        -1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f
    });
    geometry->AddAttribute(positions);

    const auto before = geometry->BoundingSphere();
    EXPECT_NEAR(before.radius, 1.0f, 1e-4f);

    positions->SetData({
        -3.0f, 0.0f, 0.0f,
         3.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f
    });

    const auto after = geometry->BoundingSphere();
    EXPECT_NEAR(after.radius, 3.0f, 1e-4f);
}

#pragma endregion

#pragma region Disposal

TEST(Geometry2, DisposeFiresCallbackOnce) {
    auto geometry = Geometry2::Create();
    auto calls = 0;

    geometry->OnDispose([&calls](vglx::Disposable*) { calls++; });
    geometry->Dispose();
    geometry->Dispose();

    EXPECT_EQ(calls, 1);
}

TEST(Geometry2, DisposeFiresOnDestruction) {
    auto calls = 0;

    {
        auto geometry = Geometry2::Create();
        geometry->OnDispose([&calls](vglx::Disposable*) { calls++; });
    }

    EXPECT_EQ(calls, 1);
}

#pragma endregion
