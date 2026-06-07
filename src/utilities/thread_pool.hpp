/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <functional>
#include <future>
#include <memory>
#include <thread>

namespace vglx {

class ThreadPool {
public:
    explicit ThreadPool(size_t thread_count = 0);

    ThreadPool(ThreadPool&&) noexcept;
    auto operator=(ThreadPool&&) noexcept -> ThreadPool&;

    ThreadPool(const ThreadPool&) = delete;
    auto operator=(const ThreadPool&) -> ThreadPool& = delete;

    auto Enqueue(std::function<void()> job) -> bool;

    template <typename F>
    auto Submit(F&& func) -> std::future<std::invoke_result_t<F>> {
        using Result = std::invoke_result_t<F>;

        auto task = std::make_shared<std::packaged_task<Result()>>(std::forward<F>(func));
        auto future = task->get_future();

        Enqueue([task]() { (*task)(); });

        return future;
    }

    ~ThreadPool();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}