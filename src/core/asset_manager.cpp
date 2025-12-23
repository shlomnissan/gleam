/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/core/asset_manager.hpp"

#include <format>
#include <fstream>
#include <functional>
#include <mutex>
#include <queue>
#include <string>

#include "utilities/file.hpp"
#include "utilities/thread_pool.hpp"

#include "vglx/asset_format.hpp"
#include "vglx/textures/texture_2d.hpp"
#include "vglx/scene/node.hpp"

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
        auto header = TextureHeader {};
        auto file = std::ifstream {path, std::ios::binary};
        auto err = std::string {};

        if (err.empty() && !file) {
            err = std::format("Unable to open image '{}'", path.string());
        } else {
            read_binary(file, header);
        }

        if (err.empty() && std::memcmp(header.magic, "TEX0", 4) != 0) {
            err = std::format("Invalid texture file '{}'", path.string());
        }

        if (err.empty() && header.version != VGLX_TEX_VER) {
            err = std::format("Unsupported file version '{}'", path.string());
        }

        // TODO: load image data

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
        auto header = MeshHeader {};
        auto file = std::ifstream {path, std::ios::binary};
        auto err = std::string {};

        if (err.empty() && !file) {
            err = std::format("Unable to open mesh '{}'", path.string());
        } else {
            read_binary(file, header);
        }

        if (err.empty() && std::memcmp(header.magic, "MSH0", 4) != 0) {
            err = std::format("Invalid mesh file '{}'", path.string());
        }

        if (err.empty() && header.version != VGLX_MSH_VER) {
            err = std::format("Unsupported file version '{}'", path.string());
        }

        // TODO: load mesh data

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