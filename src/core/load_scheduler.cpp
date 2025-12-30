/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/core/load_scheduler.hpp"

#include <mutex>
#include <queue>

#include "utilities/logger.hpp"
#include "utilities/thread_pool.hpp"

#include "vglx/loaders/mesh_loader_xyz.hpp"
#include "vglx/loaders/texture_loader_xyz.hpp"
#include "vglx/scene/node.hpp"
#include "vglx/textures/texture_2d.hpp"

namespace vglx {

struct LoadScheduler::Impl {
    std::mutex queue_mutex;
    std::queue<CommitFn> completions;
    ThreadPool pool;

    auto Post(std::function<void()> fn) -> void {
        auto lock = std::scoped_lock {queue_mutex};
        completions.push(std::move(fn));
    }

    auto Enqueue(WorkFn work, CommitFn commit) -> void {
        pool.Enqueue(
            [this, work = std::move(work), commit = std::move(commit)]() mutable {
                work();
                Post(std::move(commit));
            }
        );
    }

    auto Pump() -> void {
        auto local = std::queue<CommitFn> {};
        {
            auto lock = std::scoped_lock {queue_mutex};
            std::swap(local, completions);
        }

        while (!local.empty()) {
            local.front()(); // commit / mark ready
            local.pop();
        }
    }
};

LoadScheduler::LoadScheduler() : impl_(std::make_unique<Impl>()) {}

auto LoadScheduler::Enqueue(WorkFn work, CommitFn commit) -> void {
    impl_->Enqueue(std::move(work), std::move(commit));
}

auto LoadScheduler::Pump() -> void {
    impl_->Pump();
}

LoadScheduler::~LoadScheduler() = default;

}