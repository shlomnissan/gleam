/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/loaders/texture_loader_xyz.hpp"

#include "vglx/asset_format.hpp"
#include "vglx/core/load_scheduler.hpp"
#include "vglx/textures/texture_2d.hpp"

#include "utilities/assert.hpp"
#include "utilities/file.hpp"
#include "utilities/logger.hpp"

#include <cstdint>
#include <cstring>
#include <format>
#include <fstream>
#include <vector>

namespace vglx {

auto load_texture(const fs::path& path) -> std::expected<std::shared_ptr<Texture2D>, std::string> {
    auto file = std::ifstream {path, std::ios::binary};
    if (!file) {
        return std::unexpected(std::format("Unable to open texture '{}'", path.string()));
    }

    auto header = TextureHeader {};
    if (!read_binary(file, header)) {
        return std::unexpected(std::format("Failed to read header from '{}'", path.string()));
    }

    if (std::memcmp(header.magic, "TEX0", 4) != 0) {
        return std::unexpected(std::format("Invalid texture file '{}'", path.string()));
    }

    if (header.version != VGLX_TEX_VER) {
        return std::unexpected(std::format("Unsupported file version '{}'", path.string()));
    }

    auto data = std::vector<uint8_t>(header.pixel_data_size);
    if (!read_binary(file, data, header.pixel_data_size)) {
        return std::unexpected(std::format("Failed to read data from '{}'", path.string()));
    }

    auto out = std::make_shared<Texture2D>(Texture2D::Parameters {
        .width = header.width,
        .height = header.height,
        .data = std::move(data)
    });

    out->SetName(path.filename().string());

    return out;
}

auto TextureLoaderXYZ::Load(const fs::path& path)
  -> std::expected<std::shared_ptr<Texture2D>, std::string> {
    return load_texture(path);
}

auto TextureLoaderXYZ::LoadAsync(const fs::path& path) -> TextureLoadHandle {
    VGLX_ASSERT(load_scheduler_ != nullptr, "Null load scheduler in texture loader");

    auto state = std::make_shared<TextureLoadHandle::State>();
    auto handle = TextureLoadHandle {state};

    load_scheduler_->Enqueue(
        [state, path] {
            auto result = load_texture(path);
            if (result.has_value()) {
                state->value = std::move(result.value());
            } else {
                state->error = result.error();
                Logger::Log(LogLevel::Error, "{}", state->error);
            }
        },
        [state] {
            VGLX_ASSERT(state != nullptr, "Null in async texture state");
            state->ready = true;
        }
    );

    return handle;
}

}