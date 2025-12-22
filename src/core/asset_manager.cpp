/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/core/asset_manager.hpp"

#include <functional>
#include <mutex>
#include <queue>
#include <print>

#include "utilities/thread_pool.hpp"

namespace vglx {

struct AssetManager::Impl {
    std::mutex queue_mutex;
    std::queue<std::function<void()>> completions;
    ThreadPool pool;

    auto Post(std::function<void()> fn) -> void {
        auto lock = std::scoped_lock {queue_mutex};
        completions.push(std::move(fn));
    }

    auto Pump() -> void {
        auto local = std::queue<std::function<void()>> {};
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

AssetManager::AssetManager() : impl_(std::make_unique<Impl>()) {}

auto AssetManager::LoadTexture(const fs::path& path) -> TextureHandle {
    auto state = std::make_shared<TextureHandle::State>();
    auto handle = TextureHandle {state};

    // load texture async

    return handle;
}

auto AssetManager::LoadMesh(const fs::path& path) -> MeshHandle {
    auto state = std::make_shared<MeshHandle::State>();
    auto handle = MeshHandle {state};

    // load mesh async

    return handle;
}

auto AssetManager::Pump() -> void {
    impl_->Pump();
}



AssetManager::~AssetManager() = default;


}