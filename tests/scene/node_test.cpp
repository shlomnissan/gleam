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
    auto child = parent->Add(vglx::Node::Create());

    EXPECT_EQ(parent->Children().size(), 1);
    EXPECT_EQ(parent->Children()[0].get(), child);
}

TEST(Node, RemoveChild) {
    auto parent = vglx::Node::Create();
    auto child = parent->Add(vglx::Node::Create());

    parent->Remove(child);

    EXPECT_TRUE(parent->Children().empty());
}

TEST(Node, RemoveAllChildren) {
    auto parent = vglx::Node::Create();

    parent->Add(vglx::Node::Create());
    parent->Add(vglx::Node::Create());
    parent->RemoveAllChildren();

    EXPECT_TRUE(parent->Children().empty());
}

#pragma endregion

#pragma region Hierarchy Queries

TEST(Node, IsChild) {
    auto parent_0 = vglx::Node::Create();
    auto child_0 = parent_0->Add(vglx::Node::Create());

    auto parent_1 = vglx::Node::Create();
    auto child_1 = parent_1->Add(vglx::Node::Create());

    EXPECT_TRUE(parent_0->IsChild(child_0));
    EXPECT_FALSE(parent_0->IsChild(child_1));

    EXPECT_TRUE(parent_1->IsChild(child_1));
    EXPECT_FALSE(parent_1->IsChild(child_0));
}

TEST(Node, IsChildAfterRemoval) {
    auto parent = vglx::Node::Create();
    auto child = parent->Add(vglx::Node::Create());

    EXPECT_TRUE(parent->IsChild(child));

    parent->Remove(child);
    EXPECT_FALSE(parent->IsChild(child));
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
    auto node = vglx::Node::Create();
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
    auto child = parent->Add(vglx::Node::Create());

    parent->SetScale(2.0f);
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
    auto child = parent->Add(vglx::Node::Create());
    child->transform_auto_update = false;

    parent->SetScale(2.0f);
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
    auto child = parent->Add(vglx::Node::Create());

    parent->UpdateTransformHierarchy();

    EXPECT_FALSE(child->ShouldUpdateWorldTransform());
}

TEST(Node, MarkDetachedNodesAsTouched) {
    auto parent = vglx::Node::Create();
    auto child = parent->Add(vglx::Node::Create());

    parent->UpdateTransformHierarchy();

    auto detached = parent->Detach(child);

    EXPECT_TRUE(detached->ShouldUpdateWorldTransform());
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

TEST(Node, RemoveNonexistentChild) {
    auto parent = vglx::Node::Create();
    auto child = vglx::Node::Create();

    parent->Remove(child.get());

    EXPECT_TRUE(parent->Children().empty());
    EXPECT_EQ(child->GetParent(), nullptr);
}

#pragma endregion