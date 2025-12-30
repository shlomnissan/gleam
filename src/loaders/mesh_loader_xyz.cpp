/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/loaders/mesh_loader_xyz.hpp"

#include "vglx/core/load_scheduler.hpp"
#include "vglx/scene/node.hpp"

#include "loaders/detail/mesh_import.hpp"
#include "utilities/assert.hpp"
#include "utilities/logger.hpp"

namespace vglx {

MeshLoaderXYZ::MeshLoaderXYZ(LoadScheduler* scheduler) : load_scheduler_(scheduler) {};

auto MeshLoaderXYZ::Load(const fs::path& path)
  -> std::expected<std::shared_ptr<Node>, std::string> {
    return detail::mesh::import(path);
}

auto MeshLoaderXYZ::LoadAsync(const fs::path& path) -> MeshLoadHandle {
    VGLX_ASSERT(load_scheduler_ != nullptr, "Null load scheduler in mesh loader");

    auto state = std::make_shared<MeshLoadHandle::State>();
    auto handle = MeshLoadHandle {state};

    load_scheduler_->Enqueue(
        [state, path] {
            auto result = detail::mesh::import(path);
            if (result.has_value()) {
                state->value = std::move(result.value());
            } else {
                state->error = result.error();
                Logger::Log(LogLevel::Error, "{}", state->error);
            }
        },
        [state] {
            VGLX_ASSERT(state != nullptr, "Null in async mesh state");
            state->ready = true;
        }
    );

    return handle;
}

}