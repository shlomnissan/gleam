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

#include "utilities/logger.hpp"
#include "utilities/thread_pool.hpp"

#include "vglx/asset_format.hpp"
#include "vglx/scene/node.hpp"
#include "vglx/textures/texture_2d.hpp"

#include "loaders/mesh_loader_xyz.hpp"
#include "loaders/texture_loader_xyz.hpp"

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

    impl_->pool.Enqueue([this, state, path] {
        auto result = load_texture(path);
        auto err = std::string {};

        if (result) {
            state->value = result.value();
        } else {
            err = result.error();
            Logger::Log(LogLevel::Error, "{}", err);
        }

        impl_->Post([state = std::move(state), err = std::move(err)]() {
            if (!state) return;
            state->error = std::move(err);
            state->ready = true;
        });
    });

    return handle;
}

auto AssetManager::LoadMesh(const fs::path& path) -> MeshHandle {
    auto state = std::make_shared<MeshHandle::State>();
    auto handle = MeshHandle {state};

    impl_->pool.Enqueue([this, state, path] {
        auto result = load_mesh(path);
        auto err = std::string {};

        if (result) {
            state->value = std::move(result.value());
        } else {
            err = result.error();
            Logger::Log(LogLevel::Error, "{}", err);
        }

        impl_->Post([state = std::move(state), err = std::move(err)]() {
            if (!state) return;
            state->error = std::move(err);
            state->ready = true;
        });
    });

    return handle;
}

auto AssetManager::Pump() -> void {
    impl_->Pump();
}

AssetManager::~AssetManager() = default;

}