/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#define STB_IMAGE_IMPLEMENTATION

#include "image_import.hpp"

#include "misc/stb_image.hpp"

#include <format>

namespace vglx::detail::image {

auto import(const fs::path& path) -> std::expected<Image, std::string> {
    if (!fs::exists(path)) {
        return std::unexpected(std::format("Can't find image {}", path.string()));
    }

    stbi_set_flip_vertically_on_load(true);

    auto width = 0;
    auto height = 0;
    auto _ = 0;
    auto data = stbi_load(path.string().c_str(), &width, &height, &_, 4);

    if (!data) {
        return std::unexpected(std::format("Failed to load image {}", path.string()));
    }

    auto size = size_t(width) * height * 4;
    auto image = Image(
        std::vector<uint8_t>(data, data + size),
        width,
        height
    );

    stbi_image_free(data);

    return image;
}

}