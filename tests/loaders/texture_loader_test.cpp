/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>

#include <vglx/loaders/load_scheduler.hpp>
#include <vglx/loaders/texture_loader.hpp>
#include <vglx/textures/texture_2d.hpp>

class TextureLoaderTest : public ::testing::Test {
public:
    TextureLoaderTest()
      : loader(std::make_unique<vglx::TextureLoader>(scheduler.get())) {}

    std::unique_ptr<vglx::LoadScheduler> scheduler = std::make_unique<vglx::LoadScheduler>();
    std::unique_ptr<vglx::TextureLoader> loader;
};

TEST_F(TextureLoaderTest, LoadTexture) {
    auto result = loader->Load(ASSETS_DIR "/texture.png");
    EXPECT_TRUE(result.has_value());

    auto texture = result.value();
    EXPECT_EQ(texture->image->data.size(), 5 * 5 * 4);
    EXPECT_EQ(texture->image->width, 5);
    EXPECT_EQ(texture->image->height, 5);
}

TEST_F(TextureLoaderTest, LoadTextureInvalidFileFormat) {
    auto result =loader->Load(ASSETS_DIR "/texture.invalid");

    EXPECT_FALSE(result.has_value());
}

TEST_F(TextureLoaderTest, LoadTextureInvalidPath) {
    auto result = loader->Load(ASSETS_DIR "/invalid_path.png");

    EXPECT_FALSE(result.has_value());
}