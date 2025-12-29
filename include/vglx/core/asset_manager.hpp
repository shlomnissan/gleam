/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/core/asset_handle.hpp"

#include <filesystem>
#include <memory>
#include <string>

namespace vglx {

namespace fs = std::filesystem;

/**
 * @brief Asynchronous asset loader.
 *
 * The asset manager provides a minimal interface for loading texture and mesh assets
 * asynchronously without enforcing a global cache or ownership policy. Load
 * requests return handles immediately while file I/O is performed in the background.
 *
 * Completion is explicit. Results are not delivered via callbacks; the
 * @ref Application "application runtime" calls @ref Pump periodically to process
 * completed work and make results available through their handles. This keeps
 * ownership transfer on the main thread and avoids implicit synchronization
 * or hidden threading behavior.
 *
 * The asset manager is accessed through the shared context. Once a handle is
 * returned it is the responsibility of the application to poll it and
 * retrieve the loaded value when ready.
 *
 * @code
 * struct MyNode : public vglx::Node {
 *   vglx::TextureHandle handle_;
 *
 *   auto OnAttached(SharedContextPointer context) -> void override {
 *     handle_ = context->asset_manager->LoadTexture(
 *       "assets/my_texture.tex"
 *     );
 *   }
 *
 *   auto OnUpdate([[maybe_unused]] float delta) -> void override {
 *     if (auto result = handle_.TryTake()) {
 *       auto texture = result.value();
 *       // use texture
 *     }
 *   }
 * };
 * @endcode
 *
 * For a full overview of the asset pipeline see the
 * [Importing Assets Guide](/manual/importing_assets).
 *
 * @ingroup CoreGroup
 */
class VGLX_EXPORT AssetManager {
public:
    AssetManager();

    AssetManager(const AssetManager&) = delete;
    auto operator=(const AssetManager&) -> AssetManager& = delete;

    AssetManager(AssetManager&&) noexcept = delete;
    auto operator=(AssetManager&&) noexcept -> AssetManager& = delete;

    /**
     * @brief Initiates asynchronous loading of a 2D texture.
     *
     * The returned handle can be polled for completion. On success, ownership
     * of the loaded texture is transferred to the caller.
     *
     * @param path Filesystem path to the texture asset.
     */
    [[nodiscard]] auto LoadTexture(const fs::path& path) -> TextureHandle;

    /**
     * @brief Initiates asynchronous loading of a mesh asset.
     *
     * The returned handle yields ownership of a newly created scene node on
     * success. The exact node type depends on the asset format and importer.
     *
     * @param path Filesystem path to the mesh asset.
     */
    [[nodiscard]] auto LoadMesh(const fs::path& path) -> MeshHandle;

    /**
     * @brief Processes completed load operations.
     *
     * Moves finished background tasks into a completed state, allowing their
     * results to be retrieved via @ref AssetHandle::TryTake or
     * @ref AssetHandle::TryError. This function is expected to be called
     * regularly from the main thread.
     */
    auto Pump() -> void;

    ~AssetManager();

private:
    /// @cond INTERNAL
    struct Impl;
    std::unique_ptr<Impl> impl_;
    /// @endcond
};

}