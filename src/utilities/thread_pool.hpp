/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <functional>
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

    ~ThreadPool();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}