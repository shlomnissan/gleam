/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>

#include <vglx/geometries/geometry.hpp>

#include <vector>
#include <utility>

using enum vglx::Geometry::VertexAttributeType;

#pragma region Constructors

TEST(Geometry, DefaultConstruction) {
    const auto geometry = vglx::Geometry::Create();
    const auto attribs = geometry->Attributes();

    EXPECT_TRUE(geometry->VertexData().empty());
    EXPECT_TRUE(geometry->IndexData().empty());
    EXPECT_EQ(attribs[std::to_underlying(Position)].type, None);
    EXPECT_EQ(attribs[std::to_underlying(UV)].type, None);
    EXPECT_EQ(attribs[std::to_underlying(Color)].type, None);
    EXPECT_EQ(attribs[std::to_underlying(InstanceColor)].type, None);
    EXPECT_EQ(attribs[std::to_underlying(InstanceTransform)].type, None);
    EXPECT_EQ(attribs[std::to_underlying(Normal)].type, None);
}

TEST(Geometry, InitializeWithVertexAndIndexData) {
    const auto vertex_data = std::vector<float>{0.0f, 1.0f, 2.0f};
    const auto index_data = std::vector<unsigned int>{0, 1, 2};
    const auto geometry = vglx::Geometry::Create(vertex_data, index_data);

    EXPECT_EQ(geometry->VertexData().size(), 3);
    EXPECT_EQ(geometry->IndexData().size(), 3);
}

TEST(Geometry, InitializeWithVertexData) {
    const auto vertex_data = std::vector<float>{0.0f, 1.0f, 2.0f};
    const auto geometry = vglx::Geometry::Create(vertex_data);

    EXPECT_EQ(geometry->VertexData().size(), 3);
    EXPECT_TRUE(geometry->IndexData().empty());
}

#pragma endregion

#pragma region Attributes

TEST(Geometry, AddAttribute) {
    const auto vertex_data = std::vector<float>{0.0f, 1.0f, 2.0f};
    auto geometry = vglx::Geometry::Create(vertex_data);

    geometry->SetAttribute({.type = Position, .item_size = 3});

    const auto& attribs = geometry->Attributes();

    auto idx = std::to_underlying(vglx::Geometry::VertexAttributeType::Position);
    EXPECT_EQ(attribs[idx].type, Position);
    EXPECT_EQ(attribs[idx].item_size, 3);
}

TEST(Geometry, AddMultipleAttributes) {
    const auto vertex_data = std::vector<float>{
        0.0f, 1.0f, 2.0f, 0.33f, 0.55f
    };

    auto geometry = vglx::Geometry::Create(vertex_data);
    geometry->SetAttribute({.type = Position, .item_size = 3});
    geometry->SetAttribute({.type = UV, .item_size = 2});

    const auto& attribs = geometry->Attributes();

    EXPECT_EQ(attribs[std::to_underlying(Position)].type, Position);
    EXPECT_EQ(attribs[std::to_underlying(UV)].type, UV);
    EXPECT_EQ(attribs[std::to_underlying(Color)].type, None);
    EXPECT_EQ(attribs[std::to_underlying(InstanceColor)].type, None);
    EXPECT_EQ(attribs[std::to_underlying(InstanceTransform)].type, None);
    EXPECT_EQ(attribs[std::to_underlying(Normal)].type, None);
}

TEST(Geometry, ReturnsTrueIfAttributeExists) {
    auto geometry = vglx::Geometry::Create({
        0.0f, 1.0f, 2.0f
    });

    geometry->SetAttribute({.type = Position, .item_size = 3});

    EXPECT_TRUE(geometry->HasAttribute(Position));
}

TEST(Geometry, ReturnsFalseIfAttributeDoesNotExist) {
    auto geometry = vglx::Geometry::Create({
        0.0f, 1.0f, 2.0f
    });

    geometry->SetAttribute({.type = Position, .item_size = 3});

    EXPECT_FALSE(geometry->HasAttribute(UV));
}

#pragma endregion

#pragma region Vertex Count

TEST(Geometry, Stride) {
    const auto vertex_data = std::vector<float>{
        0.0f, 1.0f, 2.0f, 0.33f, 0.55f,
        0.0f, 1.0f, 2.0f, 0.33f, 0.55f
    };

    auto geometry = vglx::Geometry::Create(vertex_data);
    geometry->SetAttribute({.type = Position, .item_size = 3});
    geometry->SetAttribute({.type = UV, .item_size = 2});

    EXPECT_EQ(geometry->Stride(), 5);
}

TEST(Geometry, VertexCount) {
    auto vertex_data = std::vector<float> {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    auto geometry = vglx::Geometry::Create(vertex_data);
    geometry->SetAttribute({.type = Position, .item_size = 3});

    EXPECT_EQ(geometry->VertexCount(), 3);
}

#pragma endregion

#pragma region Edge Cases

TEST(Geometry, AddAttributeWithWrongItemSize) {
    const auto vertex_data = std::vector<float>{0.0f, 1.0f, 2.0f};
    auto geometry = vglx::Geometry::Create(vertex_data);

    EXPECT_DEATH({
        geometry->SetAttribute({.type = Position, .item_size = 4});
    }, ".*attribute.item_size == 3");

    EXPECT_DEATH({
        geometry->SetAttribute({.type = Normal, .item_size = 4});
    }, ".*attribute.item_size == 3");

    EXPECT_DEATH({
        geometry->SetAttribute({.type = UV, .item_size = 3});
    }, ".*attribute.item_size == 2");

    EXPECT_DEATH({
        geometry->SetAttribute({.type = Color, .item_size = 4});
    }, ".*attribute.item_size == 3");
}

TEST(Geometry, AddInternalAttributes) {
    const auto vertex_data = std::vector<float>{0.0f, 1.0f, 2.0f};
    auto geometry = vglx::Geometry::Create(vertex_data);

    EXPECT_DEATH({
        geometry->SetAttribute({.type = InstanceColor, .item_size = 3});
    }, ".*attribute.type != InstanceColor");

    EXPECT_DEATH({
        geometry->SetAttribute({.type = InstanceTransform, .item_size = 4});
    }, ".*attribute.type != InstanceTransform");
}

#pragma endregion