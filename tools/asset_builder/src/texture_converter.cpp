/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#define STB_IMAGE_IMPLEMENTATION

#include "vglx/asset_format.hpp"

#include "texture_converter.hpp"

#include <cstdint>
#include <cstring>
#include <fstream>

#include "stb_image.hpp"

auto convert_texture(
    const fs::path& input_path,
    const fs::path& output_path
) -> std::expected<void, std::string> {
    auto width = 0;
    auto height = 0;
    auto channels = 0;

    stbi_set_flip_vertically_on_load(true);
    auto data = stbi_load(input_path.string().c_str(), &width, &height, &channels, 4);
    if (!data) {
        return std::unexpected("Failed to load image: " + input_path.string());
    }

    auto header = TextureHeader {};
    std::memcpy(header.magic, "TEX0", 4);
    header.version = VGLX_TEX_VER;
    header.header_size = sizeof(TextureHeader);
    header.width = static_cast<uint32_t>(width);
    header.height = static_cast<uint32_t>(height);
    header.format = static_cast<uint32_t>(TextureFormat::TextureFormat_RGBA8);
    header.mip_levels = 1;
    header.pixel_data_size = static_cast<uint64_t>(width) * height * 4;

    if (output_path.has_parent_path()) {
        auto err = std::error_code {};
        fs::create_directories(output_path.parent_path(), err);
        if (err) return std::unexpected("Failed to create output directory");
    }

    auto out_stream = std::ofstream {output_path, std::ios::binary};
    if (!out_stream) {
        stbi_image_free(data);
        return std::unexpected("Failed to create output file: " + output_path.string());
    }

    out_stream.write(reinterpret_cast<const char*>(&header), sizeof(header));
    out_stream.write(reinterpret_cast<const char*>(data), header.pixel_data_size);
    stbi_image_free(data);

    return {};
}