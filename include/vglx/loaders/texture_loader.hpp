/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include <expected>
#include <filesystem>
#include <memory>
#include <string>

#include "vglx/loaders/load_handle.hpp"

namespace vglx {

class Texture2D;
class LoadScheduler;

namespace fs = std::filesystem;

/**
 * @brief Loader for 2D texture assets.
 *
 * Provides both synchronous and asynchronous interfaces for loading texture
 * data from disk. Supported formats include PNG, JPEG, TGA, BMP, and HDR. It is
 * intended to be accessed through the shared runtime context and used
 * directly by application or node code when texture resources are needed.
 *
 * The synchronous @ref Load method performs the entire load on the calling
 * thread and returns either a fully constructed texture or an error message.
 *
 * The asynchronous @ref LoadAsync method schedules file I/O off the main thread
 * and returns a @ref LoadHandle "TextureLoadHandle" immediately. The handle can
 * later be queried from the main thread to retrieve the loaded texture once the
 * operation completes. Ownership of the texture is transferred out of the handle
 * when it is successfully taken.
 *
 * Internally, asynchronous loading is coordinated through a
 * @ref LoadScheduler instance, which ensures that background work and main-thread
 * commits are clearly separated.
 *
 * @code
 * auto handle = context->texture_loader->LoadAsync("assets/diffuse.png");
 *
 * auto OnUpdate(float) -> void override {
 *   if (auto texture = handle.TryTake()) {
 *     material->texture_map = texture.value();
 *   }
 * }
 * @endcode
 *
 * To learn more about how textures are imported and loaded see the
 * [Importing Assets Guide](/manual/importing_assets).
 *
 * @ingroup LoadersGroup
 */

class VGLX_EXPORT TextureLoader {
public:
    explicit TextureLoader(LoadScheduler* scheduler);

    /**
     * @brief Loads a texture synchronously from an image file.
     *
     * Performs file I/O and texture creation on the calling thread. If loading
     * succeeds a fully constructed @ref Texture2D is returned. On failure
     * an error message describing the problem is returned instead.
     *
     * This method is intended for tooling, offline processing, or scenarios
     * where blocking behavior is acceptable.
     *
     * @param path Filesystem path to the texture asset.
     */
    auto Load(const fs::path& path) const -> std::expected<std::shared_ptr<Texture2D>, std::string>;

    /**
     * @brief Loads a texture asynchronously from an image file.
     *
     * Schedules file I/O work to run off the main thread and returns
     * immediately with a @ref LoadHandle "TextureLoadHandle". The handle can later
     * be polled to retrieve the loaded texture once the operation completes.
     *
     * Ownership of the texture is transferred out of the handle when it is
     * successfully taken. Errors can be retrieved explicitly from the handle
     * or will be reported through the logger.
     *
     * @param path Filesystem path to the texture asset.
     */
    auto LoadAsync(const fs::path& path) const -> TextureLoadHandle;

private:
    LoadScheduler* load_scheduler_ {nullptr};
};

}