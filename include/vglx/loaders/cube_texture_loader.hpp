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
#include "vglx/textures/cube_texture.hpp"

#include <expected>
#include <filesystem>
#include <memory>
#include <string>

namespace vglx {

namespace fs = std::filesystem;

/**
 * @brief Loader for cube map texture assets.
 *
 * Provides both synchronous and asynchronous interfaces for loading the six
 * face images of a cube map from disk. Supported formats include PNG, JPEG,
 * TGA, BMP, and HDR. It is intended to be accessed through the shared runtime
 * context and used directly by application or node code when cube map
 * resources are needed.
 *
 * The synchronous @ref Load method performs the entire load on the calling
 * thread and returns either a fully constructed cube texture or an error
 * message.
 *
 * The asynchronous @ref LoadAsync method schedules file I/O off the main
 * thread and returns a @ref LoadHandle "CubeTextureLoadHandle" immediately.
 * The handle can later be queried from the main thread to retrieve the loaded
 * texture once the operation completes. Ownership of the texture is
 * transferred out of the handle when it is successfully taken.
 *
 * Internally, asynchronous loading is coordinated through a
 * @ref LoadScheduler instance, which ensures that background work and
 * main-thread commits are clearly separated.
 *
 * @code
 * auto MyScene::OnAttached(SharedContextPointer context) -> void {
 *   skybox_handle_ = context->cube_texture_loader->LoadAsync({
 *     .positive_x = "assets/skybox/px.png",
 *     .negative_x = "assets/skybox/nx.png",
 *     .positive_y = "assets/skybox/py.png",
 *     .negative_y = "assets/skybox/ny.png",
 *     .positive_z = "assets/skybox/pz.png",
 *     .negative_z = "assets/skybox/nz.png",
 *   });
 * }
 *
 * auto MyScene::OnUpdate(float _) -> void {
 *   if (auto skybox = skybox_handle_.TryTake()) {
 *     this->background = skybox.value();
 *   }
 * }
 * @endcode
 *
 * @ingroup LoadersGroup
 */
class VGLX_EXPORT CubeTextureLoader {
public:
    /**
     * @brief Paths to the six face images that make up a cube map.
     */
    struct Parameters {
        fs::path positive_x; ///< Right face (+X).
        fs::path negative_x; ///< Left face (-X).
        fs::path positive_y; ///< Top face (+Y).
        fs::path negative_y; ///< Bottom face (-Y).
        fs::path positive_z; ///< Front face (+Z).
        fs::path negative_z; ///< Back face (-Z).
    };

    explicit CubeTextureLoader(LoadScheduler* scheduler);

    /**
     * @brief Loads a cube texture synchronously from six image files.
     *
     * Performs file I/O and texture creation on the calling thread. If loading
     * succeeds a fully constructed @ref CubeTexture is returned. On failure
     * an error message describing the problem is returned instead.
     *
     * This method is intended for tooling, offline processing, or scenarios
     * where blocking behavior is acceptable.
     *
     * @param params Paths to the six face images.
     */
    auto Load(const Parameters& params) const -> std::expected<std::shared_ptr<CubeTexture>, std::string>;

    /**
     * @brief Loads a cube texture asynchronously from six image files.
     *
     * Schedules file I/O work to run off the main thread and returns
     * immediately with a @ref LoadHandle "CubeTextureLoadHandle". The handle
     * can later be polled to retrieve the loaded texture once the operation
     * completes.
     *
     * Ownership of the texture is transferred out of the handle when it is
     * successfully taken. Errors can be retrieved explicitly from the handle
     * or will be reported through the logger.
     *
     * @param params Paths to the six face images.
     */
    auto LoadAsync(const Parameters& params) const -> CubeTextureLoadHandle;

private:
    LoadScheduler* load_scheduler_ {nullptr};
};

}