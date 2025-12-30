/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/loaders/texture_loader_xyz.hpp"

#include "vglx/core/load_scheduler.hpp"
#include "vglx/textures/texture_2d.hpp"

#include "loaders/detail/texture_import.hpp"
#include "utilities/assert.hpp"
#include "utilities/logger.hpp"

namespace vglx {

TextureLoaderXYZ::TextureLoaderXYZ(LoadScheduler* scheduler) : load_scheduler_(scheduler) {};

auto TextureLoaderXYZ::Load(const fs::path& path)
  -> std::expected<std::shared_ptr<Texture2D>, std::string> {
    return detail::texture::import(path);
}

auto TextureLoaderXYZ::LoadAsync(const fs::path& path) -> TextureLoadHandle {
    VGLX_ASSERT(load_scheduler_ != nullptr, "Null load scheduler in texture loader");

    auto state = std::make_shared<TextureLoadHandle::State>();
    auto handle = TextureLoadHandle {state};

    load_scheduler_->Enqueue(
        [state, path] {
            auto result = detail::texture::import(path);
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