/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <functional>
#include <string>
#include <vector>

#include "identity.hpp"

namespace vglx {

/// @cond INTERNAL
class Disposable : public Identity {
public:
    using OnDisposeCallback = std::function<void(const std::string&)>;

    Disposable() = default;

    // Non-copyable
    Disposable(const Disposable&) = delete;
    auto operator=(const Disposable&) -> Disposable& = delete;

    // Non-moveable
    Disposable(Disposable&&) = delete;
    auto operator=(Disposable&&) -> Disposable& = delete;

    auto Dispose() -> void {
        if (!disposed_) {
            disposed_ = true;
            for (const auto& c : dispose_callbacks_) c(UUID());
        }
    }

    [[nodiscard]] auto Disposed() -> bool { return disposed_; }

    auto OnDispose(const OnDisposeCallback& callback) {
        dispose_callbacks_.emplace_back(callback);
    }

    auto RemoveDisposeHandlers() -> void {
        dispose_callbacks_.clear();
    }

    virtual ~Disposable() {
        Dispose();
    }

private:
    std::vector<OnDisposeCallback> dispose_callbacks_;

    bool disposed_ {false};
};
/// @endcond

}
