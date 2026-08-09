/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>
#include <test_helpers.hpp>

#include <vglx/geometries/buffer_attribute.hpp>
#include <vglx/geometries/geometry.hpp>
#include <vglx/materials/unlit_material.hpp>
#include <vglx/scene/instanced_mesh2.hpp>

#include <vector>

namespace {

auto create_mesh(std::size_t count) {
    auto geometry = vglx::Geometry::Create({0.0f, 0.0f, 0.0f});
    geometry->SetAttribute({
        .type = vglx::Geometry::VertexAttributeType::Position,
        .item_size = 3
    });
    return vglx::InstancedMesh2::Create(geometry, vglx::UnlitMaterial::Create(), count);
}

auto create_translation(float x, float y, float z) {
    return vglx::Matrix4 {
        1.0f, 0.0f, 0.0f, x,
        0.0f, 1.0f, 0.0f, y,
        0.0f, 0.0f, 1.0f, z,
        0.0f, 0.0f, 0.0f, 1.0f
    };
}

}

#pragma region Construction

TEST(InstancedMesh2, ConstructorInitializesIdentityTransformsAndWhiteColors) {
    const auto mesh = create_mesh(2);

    EXPECT_EQ(mesh->GetCount(), 2);
    EXPECT_EQ(mesh->GetInstanceAttributes().size(), 2);
    EXPECT_MAT4_EQ(mesh->TransformAt(1), vglx::Matrix4::Identity());
    EXPECT_FLOAT_EQ(mesh->ColorAt(1).r, 1.0f);
    EXPECT_FLOAT_EQ(mesh->ColorAt(1).g, 1.0f);
    EXPECT_FLOAT_EQ(mesh->ColorAt(1).b, 1.0f);
}

#pragma endregion

#pragma region Transforms and Colors

TEST(InstancedMesh2, SetTransformAtRoundTripsAndBumpsVersion) {
    auto mesh = create_mesh(2);
    const auto transform = create_translation(1.0f, 2.0f, 3.0f);

    mesh->SetTransformAt(1, transform);

    EXPECT_MAT4_EQ(mesh->TransformAt(1), transform);
    EXPECT_MAT4_EQ(mesh->TransformAt(0), vglx::Matrix4::Identity());
    EXPECT_EQ(mesh->GetInstanceAttribute(vglx::BufferAttribute::kInstanceTransform)->GetVersion(), 1);
}

TEST(InstancedMesh2, SetColorAtRoundTripsAndBumpsVersion) {
    auto mesh = create_mesh(2);

    mesh->SetColorAt(0, {0.2f, 0.4f, 0.6f});

    EXPECT_FLOAT_EQ(mesh->ColorAt(0).r, 0.2f);
    EXPECT_FLOAT_EQ(mesh->ColorAt(0).g, 0.4f);
    EXPECT_FLOAT_EQ(mesh->ColorAt(0).b, 0.6f);
    EXPECT_FLOAT_EQ(mesh->ColorAt(1).r, 1.0f);
    EXPECT_EQ(mesh->GetInstanceAttribute(vglx::BufferAttribute::kInstanceColor)->GetVersion(), 1);
}

#pragma endregion

#pragma region Instance Attributes

TEST(InstancedMesh2, AddInstanceAttribute) {
    auto mesh = create_mesh(2);
    auto attribute = vglx::BufferAttribute::Create({
        .name = "a_Custom",
        .format = vglx::BufferAttribute::Format::Float32x1,
        .rate = vglx::BufferAttribute::Rate::Instance
    }, {0.0f, 1.0f});

    mesh->AddInstanceAttribute(attribute);

    EXPECT_EQ(mesh->GetInstanceAttribute("a_Custom"), attribute);
    EXPECT_EQ(mesh->GetLayoutVersion(), 1);
}

TEST(InstancedMesh2, RejectsAttributeWithVertexRate) {
    auto mesh = create_mesh(2);

    mesh->AddInstanceAttribute(vglx::BufferAttribute::Create({
        .name = "a_Custom",
        .format = vglx::BufferAttribute::Format::Float32x1,
        .rate = vglx::BufferAttribute::Rate::Vertex
    }, {0.0f, 1.0f}));

    EXPECT_EQ(mesh->GetInstanceAttribute("a_Custom"), nullptr);
    EXPECT_EQ(mesh->GetLayoutVersion(), 0);
}

TEST(InstancedMesh2, RejectsAttributeWithDuplicateName) {
    auto mesh = create_mesh(2);

    mesh->AddInstanceAttribute(vglx::BufferAttribute::Create({
        .name = vglx::BufferAttribute::kInstanceColor,
        .format = vglx::BufferAttribute::Format::Float32x3,
        .rate = vglx::BufferAttribute::Rate::Instance
    }, std::vector<float>(6, 0.0f)));

    EXPECT_EQ(mesh->GetLayoutVersion(), 0);
}

TEST(InstancedMesh2, RejectsAttributeWithElementCountMismatch) {
    auto mesh = create_mesh(2);

    // 3 elements for 2 instances
    mesh->AddInstanceAttribute(vglx::BufferAttribute::Create({
        .name = "a_Custom",
        .format = vglx::BufferAttribute::Format::Float32x1,
        .rate = vglx::BufferAttribute::Rate::Instance
    }, {0.0f, 1.0f, 2.0f}));

    EXPECT_EQ(mesh->GetInstanceAttribute("a_Custom"), nullptr);
    EXPECT_EQ(mesh->GetLayoutVersion(), 0);
}

#pragma endregion

#pragma region Bounding Volumes

TEST(InstancedMesh2, BoundingBoxEnclosesAllInstances) {
    auto mesh = create_mesh(2);
    mesh->SetTransformAt(0, create_translation(-1.0f, 0.0f, 0.0f));
    mesh->SetTransformAt(1, create_translation(1.0f, 0.0f, 0.0f));

    const auto box = mesh->BoundingBox();

    EXPECT_VEC3_EQ(box.min, {-1.0f, 0.0f, 0.0f});
    EXPECT_VEC3_EQ(box.max, {1.0f, 0.0f, 0.0f});
}

TEST(InstancedMesh2, BoundingBoxInvalidatedWhenTransformChanges) {
    auto mesh = create_mesh(2);

    const auto before = mesh->BoundingBox();
    EXPECT_VEC3_EQ(before.max, {0.0f, 0.0f, 0.0f});

    mesh->SetTransformAt(1, create_translation(2.0f, 0.0f, 0.0f));

    const auto after = mesh->BoundingBox();
    EXPECT_VEC3_EQ(after.max, {2.0f, 0.0f, 0.0f});
}

#pragma endregion
