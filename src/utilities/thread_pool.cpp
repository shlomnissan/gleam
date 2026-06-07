/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "utilities/thread_pool.hpp"

#include <algorithm>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace vglx {

struct ThreadPool::Impl {
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> jobs;
    std::mutex mutex;
    std::condition_variable cv;

    bool stop {false};

    Impl(size_t thread_count) {
        if (thread_count == 0) {
            thread_count = std::thread::hardware_concurrency();
            if (thread_count == 0) {
                thread_count = 4;
            }
            thread_count = std::max<size_t>(1, thread_count - 1);
        }

        workers.reserve(thread_count);
        for (size_t i = 0; i < thread_count; ++i) {
            workers.emplace_back([this] { WorkerLoop(); });
        }
    }

    auto WorkerLoop() -> void {
        while (true) {
            auto job = std::function<void()> {};
            {
                auto lock = std::unique_lock {mutex};

                cv.wait(lock, [this] { return!jobs.empty() || stop; });
                if (stop && jobs.empty()) {
                    return;
                }

                job = std::move(jobs.front());
                jobs.pop();
            }
            job();
        }
    }

    auto Enqueue(std::function<void()> job) -> bool {
        {
            auto lock = std::scoped_lock {mutex};
            if (stop) return false;
            jobs.push(std::move(job));
        }

        cv.notify_one();

        return true;
    }

    auto StopAndJoin() noexcept -> void {
        {
            auto lock = std::scoped_lock {mutex};
            if (stop) return;
            stop = true;
        }

        cv.notify_all();
        for (auto& t : workers) {
            if (t.joinable()) t.join();
        }

        workers.clear();
    }
};

ThreadPool::ThreadPool(size_t thread_count) : impl_(std::make_unique<Impl>(thread_count)) {}

ThreadPool::ThreadPool(ThreadPool&&) noexcept = default;

auto ThreadPool::operator=(ThreadPool&&) noexcept -> ThreadPool& = default;

auto ThreadPool::Enqueue(std::function<void()> job) -> bool {
    if (!job) return false;
    return impl_->Enqueue(std::move(job));
}

ThreadPool::~ThreadPool() {
    if (impl_) impl_->StopAndJoin();
}

}