/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>

#include <vglx/geometries/buffer_attribute.hpp>

#include <utility>
#include <vector>

using vglx::BufferAttribute;

namespace {

auto create_attribute(BufferAttribute::Format format, std::vector<float> data) {
    return BufferAttribute::Create({
        .name = "a_Test",
        .format = format,
        .rate = BufferAttribute::Rate::Vertex
    }, std::move(data));
}

}

#pragma region Formats

TEST(BufferAttribute, ComponentsPerFormat) {
    using enum BufferAttribute::Format;

    EXPECT_EQ(create_attribute(Float32x1, {0.0f})->Components(), 1);
    EXPECT_EQ(create_attribute(Float32x2, std::vector<float>(2))->Components(), 2);
    EXPECT_EQ(create_attribute(Float32x3, std::vector<float>(3))->Components(), 3);
    EXPECT_EQ(create_attribute(Float32x4, std::vector<float>(4))->Components(), 4);
    EXPECT_EQ(create_attribute(Float32x16, std::vector<float>(16))->Components(), 16);
}

TEST(BufferAttribute, ElementCount) {
    using enum BufferAttribute::Format;

    EXPECT_EQ(create_attribute(Float32x3, std::vector<float>(6))->ElementCount(), 2);
    EXPECT_EQ(create_attribute(Float32x2, std::vector<float>(6))->ElementCount(), 3);
    EXPECT_EQ(create_attribute(Float32x16, std::vector<float>(32))->ElementCount(), 2);
}

#pragma endregion

#pragma region Data

TEST(BufferAttribute, SetDataReplacesDataAndBumpsVersion) {
    auto attribute = create_attribute(BufferAttribute::Format::Float32x3, {
        1.0f, 2.0f, 3.0f
    });

    EXPECT_EQ(attribute->GetVersion(), 0);

    attribute->SetData({
        4.0f, 5.0f, 6.0f,
        7.0f, 8.0f, 9.0f
    });

    EXPECT_EQ(attribute->GetData().size(), 6);
    EXPECT_FLOAT_EQ(attribute->GetData()[0], 4.0f);
    EXPECT_EQ(attribute->ElementCount(), 2);
    EXPECT_EQ(attribute->GetVersion(), 1);
}

TEST(BufferAttribute, SetDataRejectsNonDivisibleSize) {
    auto attribute = create_attribute(BufferAttribute::Format::Float32x3, {
        1.0f, 2.0f, 3.0f
    });

    // 4 floats is not divisible by 3 components
    attribute->SetData({4.0f, 5.0f, 6.0f, 7.0f});

    EXPECT_EQ(attribute->GetData().size(), 3);
    EXPECT_FLOAT_EQ(attribute->GetData()[0], 1.0f);
    EXPECT_EQ(attribute->GetVersion(), 0);
}

#pragma endregion

#pragma region Validity

TEST(BufferAttribute, IsValidRequiresName) {
    const auto attribute = BufferAttribute::Create({
        .name = "",
        .format = BufferAttribute::Format::Float32x3,
        .rate = BufferAttribute::Rate::Vertex
    }, {1.0f, 2.0f, 3.0f});

    EXPECT_FALSE(attribute->IsValid());
}

TEST(BufferAttribute, IsValidRequiresData) {
    const auto attribute = create_attribute(BufferAttribute::Format::Float32x3, {});

    EXPECT_FALSE(attribute->IsValid());
}

TEST(BufferAttribute, IsValidWithNameAndData) {
    const auto attribute = create_attribute(BufferAttribute::Format::Float32x3, {
        1.0f, 2.0f, 3.0f
    });

    EXPECT_TRUE(attribute->IsValid());
}

#pragma endregion

#pragma region Disposal

TEST(BufferAttribute, DisposeFiresCallbackOnce) {
    auto attribute = create_attribute(BufferAttribute::Format::Float32x1, {1.0f});
    auto calls = 0;

    attribute->OnDispose([&calls](vglx::Disposable*) { calls++; });
    attribute->Dispose();
    attribute->Dispose();

    EXPECT_EQ(calls, 1);
}

TEST(BufferAttribute, DisposeFiresOnDestruction) {
    auto calls = 0;

    {
        auto attribute = create_attribute(BufferAttribute::Format::Float32x1, {1.0f});
        attribute->OnDispose([&calls](vglx::Disposable*) { calls++; });
    }

    EXPECT_EQ(calls, 1);
}

#pragma endregion
