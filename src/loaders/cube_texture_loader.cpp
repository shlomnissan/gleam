/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/loaders/cube_texture_loader.hpp"

#include "vglx/loaders/load_scheduler.hpp"

#include "loaders/detail/image_import.hpp"

#include "utilities/assert.hpp"
#include "utilities/logger.hpp"

namespace vglx {

namespace {

auto load_cube_texture(const CubeTextureLoader::Parameters& params)
  -> std::expected<std::shared_ptr<CubeTexture>, std::string> {
    auto px = detail::image::import(params.positive_x, false);
    if (!px.has_value()) return std::unexpected(px.error());

    auto nx = detail::image::import(params.negative_x, false);
    if (!nx.has_value()) return std::unexpected(nx.error());

    auto py = detail::image::import(params.positive_y, false);
    if (!py.has_value()) return std::unexpected(py.error());

    auto ny = detail::image::import(params.negative_y, false);
    if (!ny.has_value()) return std::unexpected(ny.error());

    auto pz = detail::image::import(params.positive_z, false);
    if (!pz.has_value()) return std::unexpected(pz.error());

    auto nz = detail::image::import(params.negative_z, false);
    if (!nz.has_value()) return std::unexpected(nz.error());

    auto out = CubeTexture::Create({
        .positive_x = std::move(px.value()),
        .negative_x = std::move(nx.value()),
        .positive_y = std::move(py.value()),
        .negative_y = std::move(ny.value()),
        .positive_z = std::move(pz.value()),
        .negative_z = std::move(nz.value()),
    });

    out->min_filter = Texture::MinFilter::Linear;
    out->mag_filter = Texture::MagFilter::Linear;

    return out;
}

}

CubeTextureLoader::CubeTextureLoader(LoadScheduler* scheduler) : load_scheduler_(scheduler) {}

auto CubeTextureLoader::Load(const Parameters& params) const
  -> std::expected<std::shared_ptr<CubeTexture>, std::string> {
    return load_cube_texture(params);
}

auto CubeTextureLoader::LoadAsync(const Parameters& params) const -> CubeTextureLoadHandle {
    VGLX_ASSERT(load_scheduler_ != nullptr, "Null load scheduler in cube texture loader");

    auto state = std::make_shared<CubeTextureLoadHandle::State>();
    auto handle = CubeTextureLoadHandle {state};

    load_scheduler_->Enqueue(
        [state, params] {
            auto result = load_cube_texture(params);
            if (result.has_value()) {
                state->value = std::move(result.value());
            } else {
                state->error = result.error();
                Logger::Log(LogLevel::Error, "{}", state->error);
            }
        },
        [state] {
            VGLX_ASSERT(state != nullptr, "Null in async cube texture state");
            state->ready = true;
        }
    );

    return handle;
}

}