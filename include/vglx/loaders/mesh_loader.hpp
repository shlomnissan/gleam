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

class Node;
class LoadScheduler;

namespace fs = std::filesystem;

/**
 * @brief Loader for mesh assets.
 *
 * Provides both synchronous and asynchronous interfaces for loading mesh
 * data from disk. Supported formats include OBJ. It is intended to be
 * accessed through the shared runtime context and used directly by
 * application or node code when mesh resources are needed.
 *
 * The synchronous @ref Load method performs the entire load on the calling
 * thread and returns either a fully constructed node hierarchy or an error
 * message.
 *
 * The asynchronous @ref LoadAsync method schedules file I/O off the main thread
 * and returns a @ref LoadHandle "MeshLoadHandle" immediately. The handle can
 * later be queried from the main thread to retrieve the loaded mesh once the
 * operation completes. Ownership of the returned node is transferred out of the
 * handle when it is successfully taken.
 *
 * Internally, asynchronous loading is coordinated through a
 * @ref LoadScheduler instance, which ensures that background work and main-thread
 * commits are clearly separated.
 *
 * @code
 * auto MyScene::OnAttached(SharedContextPointer context) -> void {
 *   mesh_handle_ = context->mesh_loader->LoadAsync("assets/robot.obj");
 * }
 *
 * auto MyScene::OnUpdate(float _) -> void {
 *   if (auto node = mesh_handle_.TryTake()) {
 *     // nodes are uniquely owned by the scene so we move
 *     Add(std::move(node.value()));
 *   }
 * }
 * @endcode
 *
 * To learn more about how meshes are imported and loaded see the
 * [Importing Assets Guide](/manual/importing_assets).
 *
 * @ingroup LoadersGroup
 */
class VGLX_EXPORT MeshLoader {
public:
    explicit MeshLoader(LoadScheduler* scheduler);

    /**
     * @brief Loads a mesh synchronously from a mesh file.
     *
     * Performs file I/O and mesh construction on the calling thread. If loading
     * succeeds a fully constructed @ref Node is returned. On failure an error
     * message describing the problem is returned instead.
     *
     * This method is intended for tooling, offline processing, or scenarios
     * where blocking behavior is acceptable.
     *
     * @param path Filesystem path to the mesh asset.
     */
    auto Load(const fs::path& path) const -> std::expected<std::unique_ptr<Node>, std::string>;

    /**
     * @brief Loads a mesh asynchronously from a mesh file.
     *
     * Schedules file I/O work to run off the main thread and returns
     * immediately with a @ref LoadHandle "MeshLoadHandle". The handle can later
     * be polled to retrieve the loaded mesh once the operation completes.
     *
     * Ownership of the returned node is transferred out of the handle when it is
     * successfully taken. Errors can be retrieved explicitly from the handle
     * or will be reported through the logger.
     *
     * @param path Filesystem path to the mesh asset.
     */
    auto LoadAsync(const fs::path& path) const -> MeshLoadHandle;

private:
    LoadScheduler* load_scheduler_ {nullptr};
};

}