/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <string>

namespace vglx {

class Node;
class Texture2D;

namespace fs = std::filesystem;

template <typename T>
class VGLX_EXPORT AssetHandle {
public:
    AssetHandle() = default;

    [[nodiscard]] auto TryError() -> std::optional<std::string> {
        if (!state_ || !state_->ready || state_->error.empty()) {
            return std::nullopt;
        }
        auto out = std::move(state_->error);
        state_->value.reset();
        state_->error.clear();
        return out;
    }

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
    struct State {
        bool ready {false};
        std::optional<T> value;
        std::string error;
    };

    std::shared_ptr<State> state_;

    friend class AssetManager;
    explicit AssetHandle(std::shared_ptr<State> s) : state_(std::move(s)) {}
};

using TextureHandle = AssetHandle<std::shared_ptr<Texture2D>>;
using MeshHandle = AssetHandle<std::unique_ptr<Node>>;

class VGLX_EXPORT AssetManager {
public:
    AssetManager();

    AssetManager(const AssetManager&) = delete;
    auto operator=(const AssetManager&) -> AssetManager& = delete;

    AssetManager(AssetManager&&) noexcept;
    auto operator=(AssetManager&&) noexcept -> AssetManager&;

    [[nodiscard]] auto LoadTexture(const fs::path& path) -> TextureHandle;

    [[nodiscard]] auto LoadMesh(const fs::path& path) -> MeshHandle;

    auto Pump() -> void;

    ~AssetManager();

private:
    /// @cond INTERNAL
    struct Impl;
    std::unique_ptr<Impl> impl_;
    /// @endcond
};

}