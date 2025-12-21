/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>

#include "utilities/thread_pool.hpp"

using namespace std::chrono_literals;

#pragma region Enqueue

TEST(ThreadPool, EnqueueRunsJobs) {
    auto pool = vglx::ThreadPool {4};
    auto counter = std::atomic<int> {0};

    constexpr auto kJobs = 64;

    for (auto i = 0; i < kJobs; ++i) {
        EXPECT_TRUE(
            pool.Enqueue([&counter] {
                counter.fetch_add(1, std::memory_order_relaxed);
            })
        );
    }

    for (int i = 0; i < 100 && counter.load(std::memory_order_relaxed) != kJobs; ++i) {
        std::this_thread::sleep_for(1ms);
    }

    EXPECT_EQ(counter.load(std::memory_order_relaxed), kJobs);
}

TEST(ThreadPool, EnqueueRejectsEmptyJob) {
    auto pool = vglx::ThreadPool {1};

    EXPECT_FALSE(pool.Enqueue({}));
}

#pragma endregion

#pragma region Destructor

TEST(ThreadPool, DestructorDrainsQueuedWork) {
    auto counter = std::atomic<int> {0};

    constexpr auto kJobs = 50;

    {
        auto pool = vglx::ThreadPool {2};
        for (int i = 0; i < kJobs; ++i) {
            EXPECT_TRUE(pool.Enqueue([&counter] {
                std::this_thread::sleep_for(1ms);
                counter.fetch_add(1, std::memory_order_relaxed);
            }));
        }

        // Pool goes out of scope here.
        // The implementation drains the queue before workers exit.
    }

    EXPECT_EQ(counter.load(std::memory_order_relaxed), 50);

}

#pragma endregion