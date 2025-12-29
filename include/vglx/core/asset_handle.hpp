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

using TextureHandle = AssetHandle<std::shared_ptr<Texture2D>>;

using MeshHandle = AssetHandle<std::unique_ptr<Node>>;

}