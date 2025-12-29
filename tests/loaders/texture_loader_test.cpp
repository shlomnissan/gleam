/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>
#include <print>

#include <vglx/textures/texture_2d.hpp>

#include "loaders/texture_loader_xyz.hpp"

TEST(TextureLoader, LoadTexture) {
    auto result = vglx::load_texture("assets/texture.tex");
    EXPECT_TRUE(result.has_value());

    auto texture = result.value();
    EXPECT_EQ(texture->data.size(), 5 * 5 * 4);
    EXPECT_EQ(texture->width, 5);
    EXPECT_EQ(texture->height, 5);
}

TEST(TextureLoader, LoadTextureInvalidFileFormat) {
    auto result = vglx::load_texture("assets/texture.png");

    EXPECT_EQ(false, result.has_value());
    EXPECT_EQ(result.error(), "Invalid texture file 'assets/texture.png'");
}

TEST(TextureLoader, LoadTextureInvalidPath) {
    auto result = vglx::load_texture("assets/invalid_path.tex");

    EXPECT_EQ(false, result.has_value());
    EXPECT_EQ(result.error(), "Unable to open texture 'assets/invalid_path.tex'");
}