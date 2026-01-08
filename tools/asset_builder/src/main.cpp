/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/asset_format.hpp"

#include "mesh_converter.hpp"
#include "texture_converter.hpp"

#include <algorithm>
#include <expected>
#include <filesystem>
#include <print>
#include <string>

#include "cxxopts.hpp"

namespace fs = std::filesystem;

enum class AssetType {
    Invalid,
    Texture,
    Mesh
};

auto get_asset_type(const fs::path& path) -> AssetType {
    if (
        path.extension() == ".png" ||
        path.extension() == ".jpg" ||
        path.extension() == ".jpeg"
    ) {
        return AssetType::Texture;
    }
    if (
        path.extension() == ".obj"
    ) {
        return AssetType::Mesh;
    }
    return AssetType::Invalid;
}

auto get_color_space(const cxxopts::ParseResult& options) -> std::expected<TextureColorSpace, std::string> {
    if (!options.count("color-space")) return TextureColorSpace_sRGB;

    auto color_space = options["color-space"].as<std::string>();
    std::ranges::transform(color_space, color_space.begin(), [](auto c) {
        return std::tolower(c);
    });

    if (color_space == "srgb") return TextureColorSpace_sRGB;
    if (color_space == "linear") return TextureColorSpace_Linear;

    return std::unexpected("Error: Unsupported color space");
}

auto asset_type_to_str(AssetType type) {
    return type == AssetType::Texture ? "texture" : "mesh";
}

auto main(int argc, char** argv) -> int {
    auto opts = cxxopts::Options {
        "asset_compiler",
        "Converts source assets into engine-optimized formats."
    };

    opts.add_options()
        ("i,input", "Input file (.png, .jpg, .obj)", cxxopts::value<std::string>())
        ("o,output", "Output directory", cxxopts::value<std::string>()->default_value(""))
        ("c,color-space", "Texture color space (linear, sRGB)", cxxopts::value<std::string>())
        ("h,help", "Show help");

    auto options = opts.parse(argc, argv);

    if (options.count("help") || argc == 1) {
        std::println("{}", opts.help());
        return 0;
    }

    if (!options.count("input")) {
        std::println(stderr, "Error: input file required (-i)");
        std::println("{}", opts.help());
        return 1;
    }

    auto input = fs::path(options["input"].as<std::string>());
    if (!fs::exists(input)) {
        std::println(stderr, "Error: input file does not exist: {}", input.string());
        return 1;
    }

    auto output = fs::path(options["output"].as<std::string>());
    if (output.has_filename() && output.filename().has_extension()) {
        std::println(stderr, "Error: output must be a directory not a file: {}", output.string());
        return 1;
    }

    output = output.empty()
        ? input
        : input.parent_path() / output / input.filename();

    output = fs::absolute(output).lexically_normal();

    auto asset_type = get_asset_type(input);
    auto result = std::expected<void, std::string>{};

    switch (asset_type) {
        case AssetType::Texture: {
            auto color_space = get_color_space(options);
            if (!color_space.has_value()) {
                std::println(stderr, "{}", color_space.error());
                return 1;
            }
            output.replace_extension("tex");
            result = convert_texture(input, output, color_space.value());
        }
            break;
        case AssetType::Mesh:
            output.replace_extension(".msh");
            result = convert_mesh(input, output);
            break;
        default:
            std::println(stderr, "Error: unsupported asset type for file: {}", input.string());
            return 1;
    }

    if (!result) {
        std::println(stderr, "Error: {}", result.error());
        return 1;
    }

    std::println("Generate {} {}", asset_type_to_str(asset_type), output.string());

    return 0;
}