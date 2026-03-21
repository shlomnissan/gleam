/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/loaders/load_handle.hpp"
#include "vglx/loaders/load_scheduler.hpp"
#include "vglx/textures/image.hpp"

#include <expected>
#include <filesystem>
#include <string>

namespace vglx {

namespace fs = std::filesystem;

/**
 * @brief Loader for image assets.
 *
 * Provides both synchronous and asynchronous interfaces for loading common
 * image formats (PNG, JPEG, TGA, BMP, HDR) from disk. It is intended to be
 * accessed through the shared runtime context and used directly by application
 * or node code.
 *
 * The synchronous @ref Load method performs the entire load on the calling
 * thread and returns either a fully constructed @ref Image or an error message.
 *
 * The asynchronous @ref LoadAsync method schedules file I/O off the main thread
 * and returns a @ref LoadHandle "ImageLoadHandle" immediately. The handle can
 * later be queried from the main thread to retrieve the loaded image once the
 * operation completes. Ownership of the image is transferred out of the handle
 * when it is successfully taken.
 *
 * Internally, asynchronous loading is coordinated through a
 * @ref LoadScheduler instance, which ensures that background work and main-thread
 * commits are clearly separated.
 *
 * @code
 * auto handle = context->image_loader->LoadAsync("assets/texture.png");
 *
 * auto OnUpdate(float) -> void override {
 *   if (auto image = handle.TryTake()) {
 *     // use image.value()
 *   }
 * }
 * @endcode
 *
 * @ingroup LoadersGroup
 */
class VGLX_EXPORT ImageLoader {
public:
    explicit ImageLoader(LoadScheduler* scheduler);

    /**
     * @brief Loads an image synchronously from a file.
     *
     * Performs file I/O and image decoding on the calling thread. If loading
     * succeeds a fully constructed @ref Image is returned. On failure an error
     * message describing the problem is returned instead.
     *
     * This method is intended for tooling, offline processing, or scenarios
     * where blocking behavior is acceptable.
     *
     * @param path Filesystem path to the image file.
     */
    auto Load(const fs::path& path) const -> std::expected<Image, std::string>;

    /**
     * @brief Loads an image asynchronously from a file.
     *
     * Schedules file I/O work to run off the main thread and returns
     * immediately with a @ref LoadHandle "ImageLoadHandle". The handle can later
     * be polled to retrieve the loaded image once the operation completes.
     *
     * Ownership of the image is transferred out of the handle when it is
     * successfully taken. Errors can be retrieved explicitly from the handle
     * or will be reported through the logger.
     *
     * @param path Filesystem path to the image file.
     */
    auto LoadAsync(const fs::path& path) const -> ImageLoadHandle;

private:
    LoadScheduler* load_scheduler_ {nullptr};
};

}