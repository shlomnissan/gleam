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
#include <functional>
#include <memory>
#include <string>
#include <thread>

namespace vglx {

namespace fs = std::filesystem;

/**
 * @brief Result type returned by resource loaders.
 *
 * Contains a pointer to the loaded resource or an error message.
 *
 * @related Loader
 */
template <typename T>
using LoaderResult = std::expected<T, std::string>;

/**
 * @brief Callback type used by asynchronous loaders.
 *
 * Invoked with the result of a loading operation once it completes.
 *
 * @related Loader
 */
template <typename T>
using LoaderCallback = std::function<void(LoaderResult<T>)>;

/**
 * @brief Abstract base class for resource loader types.
 *
 * Loader provides a common interface for loading engine assets from the
 * filesystem, typically in engine-optimized formats. It supports both
 * synchronous and asynchronous loading through @ref Load and @ref LoadAsync.
 *
 * Concrete loaders such as @ref TextureLoader and @ref MeshLoader implement
 * this interface to handle engine-optimized asset formats. Additional runtime
 * loaders can be implemented by deriving from this class and providing a
 * resource-specific @ref LoadImpl method.
 *
 * @ingroup LoadersGroup
 */
template <typename Resource>
class VGLX_EXPORT Loader : public std::enable_shared_from_this<Loader<Resource>> {
public:
    /**
     * @brief Loads a resource synchronously from the specified file path.
     *
     * Verifies that the file exists before attempting to load. If the file is
     * missing or an error occurs during loading, an error message is returned
     * via `std\::unexpected`.
     *
     * @param path File system path to the resource.
     */
    auto Load(const fs::path& path) const -> LoaderResult<Resource> {
        if (!fs::exists(path)) {
            return std::unexpected("File not found '" + path.string() + "'");
        }
        return LoadImpl(path);
    }

    /**
     * @brief Loads a resource asynchronously from the specified file path.
     *
     * Verifies that the file exists, then performs the loading operation on a
     * background thread. Once loading completes, the result is delivered to
     * the provided callback.
     *
     * The current implementation spawns a detached thread for each call and
     * is intended as a simple baseline. In production scenarios it is
     * recommended to integrate this with a thread pool or task system for
     * better control and efficiency.
     *
     * @param path File system path to the resource.
     * @param callback Callback that receives the result of the loading
     * operation.
     */
    auto LoadAsync(const fs::path& path, LoaderCallback<Resource> callback) const -> void {
        if (!fs::exists(path)) {
            callback(std::unexpected("File not found '" + path.string() + "'"));
            return;
        }

        auto self = this->shared_from_this();
        std::thread([self, path, callback] {
            auto result = self->LoadImpl(path);
            callback(std::move(result));
        }).detach();
    }

    /**
     * @brief Implements the resource-specific loading logic.
     *
     * Called by @ref Load and @ref LoadAsync after the file has been verified
     * to exist. Derived classes must override this method to decode the file
     * and construct the resource instance.
     *
     * @param path File system path to the resource.
     */
    [[nodiscard]] virtual auto LoadImpl(const fs::path& path) const -> LoaderResult<Resource> = 0;
};

}