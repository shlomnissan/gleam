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

auto import(const fs::path& path, bool flip_vertically) -> std::expected<std::shared_ptr<Image>, std::string> {
    stbi_set_flip_vertically_on_load_thread(flip_vertically);

    auto width = 0;
    auto height = 0;
    auto _ = 0;
    auto data = stbi_load(path.string().c_str(), &width, &height, &_, 4);
    if (!data) {
        return std::unexpected(std::format("Failed to load image {}", path.string()));
    }

    auto size = size_t(width) * height * 4;
    auto image = Image::Create({
        .data = std::vector<uint8_t>(data, data + size),
        .width = static_cast<unsigned>(width),
        .height = static_cast<unsigned>(height)
    });

    stbi_image_free(data);
    return image;
}

auto import_hdr(const fs::path& path, bool flip_vertically) -> std::expected<std::shared_ptr<Image>, std::string> {
    stbi_set_flip_vertically_on_load_thread(flip_vertically);

    auto width = 0;
    auto height = 0;
    auto _ = 0;
    auto data = stbi_loadf(path.string().c_str(), &width, &height, &_, 4);
    if (!data) {
        return std::unexpected(std::format("Failed to load image {}", path.string()));
    }

    auto size = size_t(width) * height * 4;
    auto image = Image::Create({
        .data = std::vector<float>(data, data + size),
        .width = static_cast<unsigned>(width),
        .height = static_cast<unsigned>(height)
    });

    stbi_image_free(data);
    return image;
}

auto is_hdr(const fs::path& path) -> bool {
    return stbi_is_hdr(path.string().c_str()) != 0;
}

}