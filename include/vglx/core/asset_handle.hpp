/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include <memory>
#include <optional>
#include <string>

namespace vglx {

class Node;
class Texture2D;

/**
 * @brief Handle representing the result of an asynchronous asset load.
 *
 * An asset handle represents the eventual outcome of an asynchronous load
 * request initiated through @ref AssetManager::LoadMesh "LoadMesh" or
 * @ref AssetManager::LoadTexture "LoadTexture". A handle is returned immediately
 * when loading begins and can later be queried to retrieve either the loaded
 * asset or an error message once the operation completes.
 *
 * Ownership of the loaded value is transferred out of the handle when it is
 * successfully taken. Handles are single-consumption: once a value or error is
 * retrieved, the internal state is cleared and subsequent queries return no
 * result. This makes asset handoff explicit and prevents accidental
 * double-ownership.
 *
 * A common usage pattern is to request assets in @ref Node::OnAttached and poll
 * the handle for completion during @ref Node::OnUpdate.
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
 * Calling @ref AssetHandle::TryError is optional and only required when the
 * application needs to handle failures. Errors are otherwise reported through
 * the engine logging system.
 *
 * For a full overview of the asset pipeline see the
 * [Importing Assets Guide](/manual/importing_assets).
 *
 * @ingroup CoreGroup
 */
template <typename T>
class VGLX_EXPORT AssetHandle {
public:
    AssetHandle() = default;

    /**
     * @brief Attempts to retrieve an error produced during loading.
     *
     * If the asset has finished loading and failed this returns the error
     * message and clears it from the handle. If the asset is not ready or
     * no error occurred an empty optional is returned.
     */
    [[nodiscard]] auto TryError() -> std::optional<std::string> {
        if (!state_ || !state_->ready || state_->error.empty()) {
            return std::nullopt;
        }
        auto out = std::move(state_->error);
        state_->value.reset();
        state_->error.clear();
        return out;
    }

    /**
     * @brief Attempts to take ownership of the loaded asset.
     *
     * If the asset has finished loading successfully this transfers ownership
     * of the value out of the handle and clears the internal state. If the asset
     * is not ready or failed to load an empty optional is returned.
     */
    [[nodiscard]] auto TryTake() -> std::optional<T> {
        if (!state_ || !state_->ready || !state_->value) {
            return std::nullopt;
        }
        auto out = std::move(*state_->value);
        state_->value.reset();
        state_->error.clear();
        return out;
    }

private:
    /// @cond INTERNAL
    struct State {
        bool ready {false};
        std::optional<T> value;
        std::string error;
    };

    std::shared_ptr<State> state_;

    friend class AssetManager;
    explicit AssetHandle(std::shared_ptr<State> s) : state_(std::move(s)) {}
    /// @endcond
};

/**
 * @brief Handle for asynchronously loaded textures.
 *
 * Convenience alias for an @ref AssetHandle that resolves
 * to a shared pointer to a @ref Texture2D. It is returned by
 * @ref AssetManager::LoadTexture and follows the same single-consumption
 * semantics as all asset handles.
 *
 * @related AssetHandle
 */
using TextureHandle = AssetHandle<std::shared_ptr<Texture2D>>;

/**
 * @brief Handle for asynchronously loaded mesh nodes.
 *
 * Convenience alias for an @ref AssetHandle that resolves to a
 * unique pointer to a @ref Node representing the imported mesh hierarchy. It is
 * returned by @ref AssetManager::LoadMesh and uses exclusive ownership to make
 * transfer of the loaded node explicit.
 *
 * @related AssetHandle
 */
using MeshHandle = AssetHandle<std::unique_ptr<Node>>;

}