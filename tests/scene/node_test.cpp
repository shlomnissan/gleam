/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>
#include <test_helpers.hpp>

#include <vglx/cameras/perspective_camera.hpp>
#include <vglx/scene/mesh.hpp>
#include <vglx/scene/node.hpp>

#pragma region Node Operations

TEST(Node, AddChild) {
    auto parent = vglx::Node::Create();
    auto child = vglx::Node::Create();

    parent->Add(child);

    EXPECT_EQ(parent->Children().size(), 1);
    EXPECT_EQ(parent->Children()[0], child);
}

TEST(Node, RemoveChild) {
    auto parent = vglx::Node::Create();
    auto child = vglx::Node::Create();

    parent->Add(child);
    parent->Remove(child);

    EXPECT_TRUE(parent->Children().empty());
}

TEST(Node, RemoveAllChildren) {
    auto parent = vglx::Node::Create();
    auto child1 = vglx::Node::Create();
    auto child2 = vglx::Node::Create();

    parent->Add(child1);
    parent->Add(child2);
    parent->RemoveAllChildren();

    EXPECT_TRUE(parent->Children().empty());
}

#pragma endregion

#pragma region Hierarchy Queries

TEST(Node, IsChild) {
    auto node_1 = vglx::Node::Create();
    auto node_2 = vglx::Node::Create();
    auto node_3 = vglx::Node::Create();

    node_1->Add(node_2);
    node_2->Add(node_3);

    EXPECT_TRUE(node_1->IsChild(node_2.get()));
    EXPECT_TRUE(node_1->IsChild(node_3.get()));
    EXPECT_TRUE(node_2->IsChild(node_3.get()));
    EXPECT_FALSE(node_2->IsChild(node_1.get()));
    EXPECT_FALSE(node_3->IsChild(node_1.get()));
}

TEST(Node, IsChildAfterRemoval) {
    auto parent = vglx::Node::Create();
    auto child = vglx::Node::Create();

    parent->Add(child);
    EXPECT_TRUE(parent->IsChild(child.get()));

    parent->Remove(child);
    EXPECT_FALSE(parent->IsChild(child.get()));
}

TEST(Node, IsChildSelf) {
    auto node = vglx::Node::Create();
    EXPECT_FALSE(node->IsChild(node.get()));
}

TEST(Node, IsChildWithNullptr) {
    auto node = vglx::Node::Create();
    EXPECT_FALSE(node->IsChild(nullptr));
}

#pragma endregion

#pragma region Update Transforms

TEST(Node, UpdateTransformsWithoutParent) {
    auto node = std::make_shared<vglx::Node>();
    node->SetScale(2.0f);

    node->UpdateTransformHierarchy();

    EXPECT_MAT4_EQ(node->GetWorldTransform(), {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    });
}

TEST(Node, UpdateTransformsWithParent) {
    auto parent = vglx::Node::Create();
    auto child = vglx::Node::Create();

    parent->SetScale(2.0f);
    parent->Add(child);

    parent->UpdateTransformHierarchy();

    EXPECT_MAT4_EQ(child->GetWorldTransform(), {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    });
}

TEST(Node, DisableTransformAutoUpdate) {
    auto parent = vglx::Node::Create();
    auto child = vglx::Node::Create();
    child->transform_auto_update = false;

    parent->SetScale(2.0f);
    parent->Add(child);
    parent->UpdateTransformHierarchy();

    EXPECT_MAT4_EQ(parent->GetWorldTransform(), {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    });

    EXPECT_MAT4_EQ(child->GetWorldTransform(), {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    });
}

TEST(Node, MarkTransformedNodeAsUntouched) {
    auto parent = vglx::Node::Create();
    auto child = vglx::Node::Create();

    parent->Add(child);
    parent->UpdateTransformHierarchy();

    EXPECT_FALSE(child->ShouldUpdateWorldTransform());
}

TEST(Node, MarkDetachedNodesAsTouched) {
    auto parent = vglx::Node::Create();
    auto child = vglx::Node::Create();

    parent->Add(child);
    parent->UpdateTransformHierarchy();

    parent->Remove(child);

    EXPECT_TRUE(child->ShouldUpdateWorldTransform());
}

#pragma endregion

#pragma region ShouldUpdate Checks

TEST(Node, ShouldUpdateTransformWhenDirty) {
    auto node = vglx::Node::Create();
    node->SetScale(0.5f);

    EXPECT_TRUE(node->ShouldUpdateWorldTransform());
}

#pragma endregion

#pragma region Edge Cases

TEST(Node, AddChildWithExistingParent) {
    auto parent1 = vglx::Node::Create();
    auto parent2 = vglx::Node::Create();
    auto child = vglx::Node::Create();

    parent1->Add(child);
    parent2->Add(child);

    EXPECT_EQ(parent1->Children().size(), 0);
    EXPECT_EQ(parent2->Children().size(), 1);
    EXPECT_EQ(child->Parent(), parent2.get());
}

TEST(Node, RemoveNonexistentChild) {
    auto parent = vglx::Node::Create();
    auto child = vglx::Node::Create();

    parent->Remove(child);

    EXPECT_TRUE(parent->Children().empty());
    EXPECT_EQ(child->Parent(), nullptr);
}

#pragma endregion