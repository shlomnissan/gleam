/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/loaders/load_handle.hpp"

#include <filesystem>
#include <functional>
#include <memory>
#include <string>

namespace vglx {

namespace fs = std::filesystem;

/**
 * @brief Schedules asynchronous load work and commits results on the main thread.
 *
 * A low-level utility used by loader implementations to execute blocking work off
 * the main thread while ensuring that ownership transfer and engine-facing object
 * creation occur on the main thread.
 *
 * The runtime owns a single scheduler instance and calls @ref Pump from the main
 * loop to process completed loads. References to the scheduler are provided to
 * loader instances when the shared context is constructed.
 *
 * Applications built on top of the @ref Application "application runtime" do not
 * interact with this class directly. It is exposed primarily to support custom
 * runtimes and direct integration scenarios.
 *
 * To learn more about loaders see the [Importing Assets Guide](/manual/importing_assets).
 *
 * @ingroup LoadersGroup
 */
class VGLX_EXPORT LoadScheduler {
public:
    /**
     * @brief Function executed on a worker thread to perform blocking load work.
     *
     * The work function should avoid touching scene state or GPU objects. It
     * typically reads from disk and fills a shared handle state.
     */
    using WorkFn = std::function<void()>;

    /**
     * @brief Function executed on the Pump thread to finalize and publish results.
     *
     * Commit runs when @ref Pump is called and is intended for main-thread-only
     * performing final handoff to application-visible state.
     */
    using CommitFn = std::function<void()>;

    LoadScheduler();

    LoadScheduler(const LoadScheduler&) = delete;
    auto operator=(const LoadScheduler&) -> LoadScheduler& = delete;

    LoadScheduler(LoadScheduler&&) noexcept = delete;
    auto operator=(LoadScheduler&&) noexcept -> LoadScheduler& = delete;

    /**
     * @brief Enqueues a load job.
     *
     * Schedules `work` to run asynchronously and registers `commit` to be
     * executed later.
     *
     * @param work Worker-thread phase.
     * @param commit Main-thread commit phase.
     */
    auto Enqueue(WorkFn work, CommitFn commit) -> void;

    /**
     * @brief Executes pending commit functions.
     *
     * Pumps the completion queue and runs commit callbacks on the calling thread.
     * This is typically invoked once per frame by the runtime.
     */
    auto Pump() -> void;

    ~LoadScheduler();

private:
    /// @cond INTERNAL
    struct Impl;
    std::unique_ptr<Impl> impl_;
    /// @endcond
};

}