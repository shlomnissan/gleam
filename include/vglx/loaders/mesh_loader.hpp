/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/loaders/loader.hpp"

#include <filesystem>
#include <memory>

namespace vglx {

class Node;

namespace fs = std::filesystem;

/**
 * @brief Callback type for receiving loaded mesh nodes.
 *
 * @related MeshLoader
 */
using MeshCallback = std::function<void(std::unique_ptr<Node>)>;

/**
 * @brief Loads mesh data from engine-optimized files.
 *
 * MeshLoader is a concrete @ref Loader implementation that reads the
 * engine's custom `.msh` format from disk and constructs a node hierarchy
 * containing one or more static mesh objects and their associated materials.
 * It exposes both synchronous and asynchronous loading through the base
 * class API.
 *
 * The returned resource is a @ref Node that acts as a group container for all
 * meshes encoded within the `.msh` file. This node can be attached directly to
 * the scene graph or inserted under an existing node.
 *
 * You can convert common 3D model formats (for example OBJ or glTF) into `.msh`
 * files using the `asset_builder` tool located in the engine's `tools`
 * directory. The `.msh` format stores geometry, materials, and metadata in a
 * compact layout optimized for fast loading at runtime.
 * See [Importing Assets](/manual/importing_assets) to learn more.
 *
 * Explicit instantiation of this class is discouraged due to lifetime concerns
 * in the current architecture, particularly when used with asynchronous
 * loading. Instead, obtain a reference to the loader through
 * @ref Node::OnAttached, which provides access to the owning context and its
 * loader instances.
 *
 * @note Loaders use `std::expected` for error values. Always check the result
 * of loading operations and handle failure cases appropriately.
 *
 * @code
 * auto MyNode::OnAttached(SharedContextPointer context) -> void override {
 *   context->mesh_loader->LoadAsync(
 *     "assets/my_model.msh",
 *     [this](auto result) {
 *       if (result) {
 *         this->Add(result.value());
 *       } else {
 *         std::println(stderr, "{}", result.error());
 *       }
 *     }
 *   );
 * }
 * @endcode
 *
 * @ingroup LoadersGroup
 */
class VGLX_EXPORT MeshLoader : public Loader<std::unique_ptr<Node>> {
public:
    /**
     * @brief Creates a shared instance of @ref MeshLoader.
     *
     * The constructor is private to ensure the loader is always owned by a
     * `std\::shared_ptr`. This is required because the base @ref Loader class
     * inherits from `std\::enable_shared_from_this`, which relies on the loader
     * being managed by a shared pointer for safe use during asynchronous loading.
     */
    [[nodiscard]] static auto Create() -> std::shared_ptr<MeshLoader> {
        return std::shared_ptr<MeshLoader>(new MeshLoader());
    }

private:
    /// @cond INTERNAL
    MeshLoader() = default;

    [[nodiscard]] auto LoadImpl(const fs::path& path) const -> LoaderResult<std::unique_ptr<Node>> override;
    /// @endcond
};

}