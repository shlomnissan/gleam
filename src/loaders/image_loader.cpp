/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/loaders/image_loader.hpp"

#include "loaders/detail/image_import.hpp"
#include "utilities/assert.hpp"
#include "utilities/logger.hpp"

#include <algorithm>
#include <array>
#include <memory>

namespace vglx {

namespace {

const std::array<std::string, 6> exts {
    ".png", ".jpg", ".jpeg", ".tga", ".bmp", ".hdr"
};

auto load_image(const fs::path& path) -> std::expected<std::shared_ptr<Image>, std::string> {
    if (!fs::exists(path)) {
        return std::unexpected(std::format("Can't find image {}", path.string()));
    }

    auto ext = path.extension().string();
    if (std::ranges::find(exts, ext) == exts.end()) {
        return std::unexpected(std::format("Unsupported file extension {}", ext));
    }

    return detail::image::import(path);
}

}

ImageLoader::ImageLoader(LoadScheduler* scheduler) : load_scheduler_(scheduler) {}

auto ImageLoader::Load(const fs::path& path) const -> std::expected<std::shared_ptr<Image>, std::string> {
    return load_image(path);
}

auto ImageLoader::LoadAsync(const fs::path& path) const -> ImageLoadHandle {
    VGLX_ASSERT(load_scheduler_ != nullptr, "Null load scheduler in image loader");

    auto state = std::make_shared<ImageLoadHandle::State>();
    auto handle = ImageLoadHandle {state};

    load_scheduler_->Enqueue(
        [state, path] {
            auto result = load_image(path);
            if (result.has_value()) {
                state->value = std::move(result.value());
            } else {
                state->error = result.error();
                Logger::Log(LogLevel::Error, "{}", state->error);
            }
        },
        [state] {
            VGLX_ASSERT(state != nullptr, "Null in async image state");
            state->ready = true;
        }
    );

    return handle;
}

}