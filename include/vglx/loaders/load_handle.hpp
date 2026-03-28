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

class CubeTexture;
class Image;
class Node;
class Texture2D;

/**
 * @brief Handle representing the result of an asynchronous load request.
 *
 * `LoadHandle` is returned immediately when an asynchronous load is initiated and
 * represents the eventual outcome of that request. It can be polled to retrieve
 * either a loaded value or an error message once the loader finishes and the
 * result is committed.
 *
 * Ownership is transferred out of the handle when the value is successfully taken.
 * Handles are single-consumption: once a value or error is retrieved the internal
 * state is cleared and subsequent calls return no result. This makes the handoff
 * explicit at the call site and avoids accidental double-ownership.
 *
 * Typical usage is to load in @ref Node::OnAttached and poll for completion
 * in @ref Node::OnUpdate.
 *
 * @code
 * struct MyNode : public vglx::Node {
 *   vglx::TextureLoadHandle handle_;
 *
 *   auto OnAttached(SharedContextPointer context) -> void override {
 *     handle_ = context->texture_loader->LoadAsync("assets/albedo.png");
 *   }
 *
 *   auto OnUpdate([[maybe_unused]] float delta) -> void override {
 *     if (auto texture = handle_.TryTake()) {
 *       // use *texture
 *     }
 *   }
 * };
 * @endcode
 *
 * To learn more about loaders see the [Importing Assets Guide](/manual/importing_assets).
 *
 * @ingroup LoadersGroup
 */
template <typename T>
class VGLX_EXPORT LoadHandle {
public:
    LoadHandle() = default;

    /**
     * @brief Attempts to take ownership of the loaded asset.
     *
     * If the asset has finished loading successfully, this transfers ownership
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

    /**
     * @brief Attempts to retrieve an error produced during loading.
     *
     * If the asset has finished loading and failed this returns the error
     * message and clears it from the handle. If the asset is not ready or
     * no error occurred an empty optional is returned.
     *
     * Use this method only if you want to explicitly handle load failures.
     * Otherwise, errors are reported through the logger.
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

    /// @cond INTERNAL
    struct State {
        bool ready {false};
        std::optional<T> value;
        std::string error;
    };

    std::shared_ptr<State> state_;

    explicit LoadHandle(std::shared_ptr<State> s) : state_(std::move(s)) {}
    /// @endcond
};

/**
 * @brief Handle type returned by asynchronous texture load requests.
 *
 * Represents the eventual result of @ref TextureLoader::LoadAsync.
 *
 * @related LoadHandle
 */
using TextureLoadHandle = LoadHandle<std::shared_ptr<Texture2D>>;

/**
 * @brief Handle type returned by asynchronous cube texture load requests.
 *
 * Represents the eventual result of @ref CubeTextureLoader::LoadAsync.
 *
 * @related LoadHandle
 */
using CubeTextureLoadHandle = LoadHandle<std::shared_ptr<CubeTexture>>;

/**
 * @brief Handle type returned by asynchronous mesh load requests.
 *
 * Represents the eventual result of @ref MeshLoader::LoadAsync.
 *
 * @related LoadHandle
 */
using MeshLoadHandle = LoadHandle<std::unique_ptr<Node>>;

/**
 * @brief Handle type returned by asynchronous image load requests.
 *
 * Represents the eventual result of @ref ImageLoader::LoadAsync.
 *
 * @related LoadHandle
 */
using ImageLoadHandle = LoadHandle<std::shared_ptr<Image>>;

}