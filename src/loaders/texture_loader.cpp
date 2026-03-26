/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/loaders/texture_loader.hpp"

#include "vglx/loaders/load_scheduler.hpp"
#include "vglx/textures/texture_2d.hpp"

#include "loaders/detail/image_import.hpp"

#include "utilities/assert.hpp"
#include "utilities/logger.hpp"

namespace vglx {

namespace {

auto load_texture(const fs::path& path) -> std::expected<std::shared_ptr<Texture2D>, std::string> {
    auto result = detail::image::import(path);
    if (!result.has_value()) {
        return std::unexpected(result.error());
    }

    auto out = Texture2D::Create(result.value());
    out->generate_mipamps = true;
    out->min_filter = Texture::MinFilter::LinearMipmapLinear;
    out->mag_filter = Texture::MagFilter::Linear;
    out->SetName(path.filename().string());
    return out;
}

}

TextureLoader::TextureLoader(LoadScheduler* scheduler) : load_scheduler_(scheduler) {};

auto TextureLoader::Load(const fs::path& path) const
  -> std::expected<std::shared_ptr<Texture2D>, std::string> {
    return load_texture(path);
}

auto TextureLoader::LoadAsync(const fs::path& path) const -> TextureLoadHandle {
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