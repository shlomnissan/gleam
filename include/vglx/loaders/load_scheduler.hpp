/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/loaders/load_handle.hpp"

#include <filesystem>
#include <functional>
#include <memory>
#include <string>

namespace vglx {

namespace fs = std::filesystem;

class LoadScheduler {
public:
    using WorkFn = std::function<void()>;
    using CommitFn = std::function<void()>;

    LoadScheduler();

    LoadScheduler(const LoadScheduler&) = delete;
    auto operator=(const LoadScheduler&) -> LoadScheduler& = delete;

    LoadScheduler(LoadScheduler&&) noexcept = delete;
    auto operator=(LoadScheduler&&) noexcept -> LoadScheduler& = delete;

    auto Enqueue(WorkFn work, CommitFn commit) -> void;

    auto Pump() -> void;

    ~LoadScheduler();

private:
    /// @cond INTERNAL
    struct Impl;
    std::unique_ptr<Impl> impl_;
    /// @endcond
};

}