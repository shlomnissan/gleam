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
class VGLX_EXPORT Handle {
public:
    Handle() = default;

    auto Handled() -> void { state_.reset(); }

    [[nodiscard]] auto IsReady() const -> bool { return state_ && state_->ready; }

    [[nodiscard]] auto HasError() const -> bool { return state_ && !state_->error.empty(); }

    [[nodiscard]] auto Error() const -> const std::string& {
        static const std::string kEmpty {};
        return state_ ? state_->error : kEmpty;
    }

    [[nodiscard]] auto Value() -> T {
        if (!state_ || !state_->value) return T {};
        T out = std::move(*state_->value);
        state_->value.reset();
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
    explicit Handle(std::shared_ptr<State> s) : state_(std::move(s)) {}
};

class VGLX_EXPORT AssetManager {
public:
    using TextureHandle = Handle<std::shared_ptr<Texture2D>>;
    using MeshHandle = Handle<std::unique_ptr<Node>>;

    AssetManager();

    AssetManager(const AssetManager&) = delete;
    auto operator=(const AssetManager&) -> AssetManager& = delete;

    AssetManager(AssetManager&&) noexcept = default;
    auto operator=(AssetManager&&) noexcept -> AssetManager& = default;

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