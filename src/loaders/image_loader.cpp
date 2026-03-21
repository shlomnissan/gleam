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

std::array<std::string, 6> supported_file_ext {
    ".png", ".jpg", ".jpeg", ".tga", ".bmp", ".hdr"
};

auto is_supported_file_ext(const fs::path& path) {
    return std::ranges::find(
        supported_file_ext, path.extension().string()
    ) != supported_file_ext.end();
}

}

ImageLoader::ImageLoader(LoadScheduler* scheduler) : load_scheduler_(scheduler) {}


auto ImageLoader::Load(const fs::path& path) const -> std::expected<Image, std::string> {
    if (!is_supported_file_ext(path)) {
        return std::unexpected("Unsupported file extension");
    }
    return detail::image::import(path);
}

auto ImageLoader::LoadAsync(const fs::path& path) const -> ImageLoadHandle {
    VGLX_ASSERT(load_scheduler_ != nullptr, "Null load scheduler in image loader");

    auto state = std::make_shared<ImageLoadHandle::State>();
    auto handle = ImageLoadHandle {state};

    if (!is_supported_file_ext(path)) {
        state->error = "Unsupported file extension";
        state->ready = true;
        return handle;
    }

    load_scheduler_->Enqueue(
        [state, path] {
            auto result = detail::image::import(path);
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