/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>

#include <vglx/geometries/buffer_attribute.hpp>

#include <string>
#include <utility>
#include <vector>

namespace {

auto create_attribute(vglx::BufferAttribute::Format format, std::vector<float> data) {
    return vglx::BufferAttribute::Create({
        .name = "a_Test",
        .format = format,
        .rate = vglx::BufferAttribute::Rate::Vertex
    }, std::move(data));
}

}

#pragma region Construction

TEST(BufferAttribute, ConstructorSetsNameFormatAndRate) {
    const auto attribute = vglx::BufferAttribute::Create({
        .name = "a_Test",
        .format = vglx::BufferAttribute::Format::Float32x3,
        .rate = vglx::BufferAttribute::Rate::Instance
    }, {1.0f, 2.0f, 3.0f});

    EXPECT_EQ(attribute->name, "a_Test");
    EXPECT_EQ(attribute->format, vglx::BufferAttribute::Format::Float32x3);
    EXPECT_EQ(attribute->rate, vglx::BufferAttribute::Rate::Instance);
}

#pragma endregion

#pragma region Formats

TEST(BufferAttribute, ComponentsPerFormat) {
    using enum vglx::BufferAttribute::Format;

    EXPECT_EQ(create_attribute(Float32x1, {0.0f})->Components(), 1);
    EXPECT_EQ(create_attribute(Float32x2, std::vector<float>(2))->Components(), 2);
    EXPECT_EQ(create_attribute(Float32x3, std::vector<float>(3))->Components(), 3);
    EXPECT_EQ(create_attribute(Float32x4, std::vector<float>(4))->Components(), 4);
    EXPECT_EQ(create_attribute(Float32x16, std::vector<float>(16))->Components(), 16);
}

TEST(BufferAttribute, ElementCount) {
    using enum vglx::BufferAttribute::Format;

    EXPECT_EQ(create_attribute(Float32x3, std::vector<float>(6))->ElementCount(), 2);
    EXPECT_EQ(create_attribute(Float32x2, std::vector<float>(6))->ElementCount(), 3);
    EXPECT_EQ(create_attribute(Float32x16, std::vector<float>(32))->ElementCount(), 2);
}

#pragma endregion

#pragma region Data

TEST(BufferAttribute, SetDataReplacesDataAndBumpsVersion) {
    auto attribute = create_attribute(vglx::BufferAttribute::Format::Float32x3, {
        1.0f, 2.0f, 3.0f,
        4.0f, 5.0f, 6.0f
    });

    EXPECT_EQ(attribute->GetVersion(), 0);

    attribute->SetData({
        7.0f, 8.0f, 9.0f,
        10.0f, 11.0f, 12.0f
    });

    EXPECT_EQ(attribute->GetData().size(), 6);
    EXPECT_FLOAT_EQ(attribute->GetData()[0], 7.0f);
    EXPECT_EQ(attribute->ElementCount(), 2);
    EXPECT_EQ(attribute->GetVersion(), 1);
}

TEST(BufferAttribute, SetDataRejectsElementCountChange) {
    auto attribute = create_attribute(vglx::BufferAttribute::Format::Float32x3, {
        1.0f, 2.0f, 3.0f
    });

    attribute->SetData({
        4.0f, 5.0f, 6.0f,
        7.0f, 8.0f, 9.0f
    });

    EXPECT_EQ(attribute->GetData().size(), 3);
    EXPECT_FLOAT_EQ(attribute->GetData()[0], 1.0f);
    EXPECT_EQ(attribute->ElementCount(), 1);
    EXPECT_EQ(attribute->GetVersion(), 0);
}

TEST(BufferAttribute, SetDataRejectsNonDivisibleSize) {
    auto attribute = create_attribute(vglx::BufferAttribute::Format::Float32x3, {
        1.0f, 2.0f, 3.0f
    });

    // 4 floats is not divisible by 3 components
    attribute->SetData({4.0f, 5.0f, 6.0f, 7.0f});

    EXPECT_EQ(attribute->GetData().size(), 3);
    EXPECT_FLOAT_EQ(attribute->GetData()[0], 1.0f);
    EXPECT_EQ(attribute->GetVersion(), 0);
}

TEST(BufferAttribute, WriteReplacesRangeAndBumpsVersion) {
    auto attribute = create_attribute(vglx::BufferAttribute::Format::Float32x3, {
        1.0f, 2.0f, 3.0f,
        4.0f, 5.0f, 6.0f
    });

    const auto values = std::vector<float> {7.0f, 8.0f, 9.0f};
    attribute->Write(3, values);

    EXPECT_FLOAT_EQ(attribute->GetData()[0], 1.0f);
    EXPECT_FLOAT_EQ(attribute->GetData()[2], 3.0f);
    EXPECT_FLOAT_EQ(attribute->GetData()[3], 7.0f);
    EXPECT_FLOAT_EQ(attribute->GetData()[5], 9.0f);
    EXPECT_EQ(attribute->GetVersion(), 1);
}

TEST(BufferAttribute, WriteRejectsOutOfBoundsRange) {
    auto attribute = create_attribute(vglx::BufferAttribute::Format::Float32x3, {
        1.0f, 2.0f, 3.0f
    });

    // 3 values at offset 1 exceed a data size of 3
    const auto values = std::vector<float> {7.0f, 8.0f, 9.0f};
    attribute->Write(1, values);

    EXPECT_FLOAT_EQ(attribute->GetData()[1], 2.0f);
    EXPECT_EQ(attribute->GetVersion(), 0);
}

TEST(BufferAttribute, WriteWithEmptyValuesIsNoOp) {
    auto attribute = create_attribute(vglx::BufferAttribute::Format::Float32x3, {
        1.0f, 2.0f, 3.0f
    });

    attribute->Write(0, {});

    EXPECT_FLOAT_EQ(attribute->GetData()[0], 1.0f);
    EXPECT_EQ(attribute->GetVersion(), 0);
}

#pragma endregion

#pragma region Validity

TEST(BufferAttribute, IsValidRequiresName) {
    const auto attribute = vglx::BufferAttribute::Create({
        .name = "",
        .format = vglx::BufferAttribute::Format::Float32x3,
        .rate = vglx::BufferAttribute::Rate::Vertex
    }, {1.0f, 2.0f, 3.0f});

    EXPECT_FALSE(attribute->IsValid());
}

TEST(BufferAttribute, IsValidRequiresData) {
    const auto attribute = create_attribute(vglx::BufferAttribute::Format::Float32x3, {});

    EXPECT_FALSE(attribute->IsValid());
}

TEST(BufferAttribute, IsValidWithNameAndData) {
    const auto attribute = create_attribute(vglx::BufferAttribute::Format::Float32x3, {
        1.0f, 2.0f, 3.0f
    });

    EXPECT_TRUE(attribute->IsValid());
}

#pragma endregion

#pragma region Disposal

TEST(BufferAttribute, DisposeFiresCallbackOnce) {
    auto attribute = create_attribute(vglx::BufferAttribute::Format::Float32x1, {1.0f});
    auto calls = 0;

    attribute->OnDispose([&calls](const std::string&) { calls++; });
    attribute->Dispose();
    attribute->Dispose();

    EXPECT_EQ(calls, 1);
}

TEST(BufferAttribute, DisposeFiresOnDestruction) {
    auto calls = 0;

    {
        auto attribute = create_attribute(vglx::BufferAttribute::Format::Float32x1, {1.0f});
        attribute->OnDispose([&calls](const std::string&) { calls++; });
    }

    EXPECT_EQ(calls, 1);
}

#pragma endregion
