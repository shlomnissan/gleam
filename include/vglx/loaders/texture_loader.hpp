/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/loaders/loader.hpp"
#include "vglx/textures/texture_2d.hpp"

#include <filesystem>
#include <memory>

namespace vglx {

namespace fs = std::filesystem;

/**
 * @brief Callback type for receiving loaded textures.
 *
 * @related TextureLoader
 */
using TextureCallback = std::function<void(std::shared_ptr<Texture2D>)>;

/**
 * @brief Loads 2D textures from engine-optimized files.
 *
 * TextureLoader is a concrete @ref Loader implementation that reads the
 * engine's custom `.tex` format from disk and constructs @ref Texture2D
 * resources. It exposes both synchronous and asynchronous loading through the
 * base class API.
 *
 * You can convert standard image formats (for example PNG or JPG) into `.tex`
 * files using the `asset_builder` CLI located in the engine's `tools`
 * directory. The `.tex` format stores texture data and metadata in a layout
 * optimized for fast loading at runtime.
 * See [Importing Assets Guide](/manual/importing_assets) to learn more.
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
 *   context->texture_loader->LoadAsync(
 *     "assets/my_texture.tex",
 *     [this](auto result) {
 *       if (result) {
 *         texture_ = result.value();
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
class VGLX_EXPORT TextureLoader : public Loader<std::shared_ptr<Texture2D>> {
public:
    /**
     * @brief Creates a shared instance of @ref TextureLoader.
     *
     * The constructor is private to ensure the loader is always owned by a
     * `std\::shared_ptr`. This is required because the base @ref Loader class
     * inherits from `std\::enable_shared_from_this`, which relies on the loader
     * being managed by a shared pointer for safe use during asynchronous loading.
     */
    [[nodiscard]] static auto Create() -> std::shared_ptr<TextureLoader> {
        return std::shared_ptr<TextureLoader>(new TextureLoader());
    }

private:
    /// @cond INTERNAL
    TextureLoader() = default;

    [[nodiscard]] auto LoadImpl(const fs::path& path) const -> LoaderResult<std::shared_ptr<Texture2D>> override;
    /// @endcond
};

}