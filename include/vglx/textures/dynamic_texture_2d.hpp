/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/textures/texture.hpp"

#include <cstdint>
#include <memory>
#include <span>
#include <vector>

namespace vglx {

/**
 * @brief Represents a two-dimensional texture with GPU-allocated storage.
 *
 * A dynamic 2D texture is intended for textures that are created at runtime and
 * updated incrementally (for example, render targets, streaming tiles, lookup
 * tables, or per-frame CPU uploads). Unlike @ref Texture2D, it does not own
 * persistent CPU-side image data. Instead, it exposes methods to update regions
 * of the texture after creation.
 *
 * @code
 * auto MyScene::OnAttached(SharedContextPointer context) -> void {
 *   dynamic_texture_ = vglx::DynamicTexture2D::Create({
 *     .width = 1024,
 *     .height = 1024,
 *     .mips = 1,
 *     .format = vglx::Texture::Format::RGBA8,
 *     .color_space = vglx::Texture::ColorSpace::Linear,
 *   });
 * }
 *
 * auto MyScene::OnUpdate(float _) -> void {
 *   auto patch_bytes = get_image_subregion();
 *
 *   dynamic_texture_->UpdateSubregion(
 *     0, // mip level
 *     0, 0, // x, y offset
 *     64, 64, // width, height
 *     patch_bytes
 *   );
 * }
 * @endcode
 *
 * See @ref LoadImage for loading raw pixel data.
 *
 * @ingroup TexturesGroup
 */
class VGLX_EXPORT DynamicTexture2D : public Texture {
public:
    /**
     * @brief Parameters for constructing a @ref DynamicTexture2D object.
     */
    struct Parameters {
        int width; ///< Width in pixels.
        int height; ///< Height in pixels.
        unsigned int mips {1}; ///< Number of mip levels to allocate.
        Format format {Format::RGBA8}; ///< Texture storage format.
        ColorSpace color_space {ColorSpace::Linear}; ///< Color space for sampling.
    };

    /// @brief Texture width in pixels.
    const int width;

    /// @brief Texture height in pixels.
    const int height;

    /// @brief Number of allocated mip levels.
    const unsigned int mips;

    /**
     * @brief Constructs a dynamic 2D texture.
     *
     * Allocates GPU storage for the requested dimensions, mip count, and format.
     * Data can be uploaded later through @ref UpdateSubregion.
     *
     * @param params @ref DynamicTexture2D::Parameters "Initialization parameters"
     * for constructing the texture.
     */
    explicit DynamicTexture2D(const Parameters& params);

    /**
     * @brief Creates a shared instance of @ref DynamicTexture2D.
     *
     * @param params @ref DynamicTexture2D::Parameters "Initialization parameters"
     * for constructing the texture.
     */
    [[nodiscard]] static auto Create(const Parameters& params)
      -> std::shared_ptr<DynamicTexture2D> {
        return std::make_shared<DynamicTexture2D>(params);
    }

    /**
     * @brief Identifies this texture as @ref Texture::Type "Texture::Type::DynamicTexture2D".
     */
    [[nodiscard]] auto GetType() const -> Texture::Type override {
        return Texture::Type::DynamicTexture2D;
    }

    /**
     * @brief Updates a rectangular subregion of the texture.
     *
     * Queues an upload for a region of the given mip level. This is typically
     * used for incremental CPU updates such as streaming tiles or procedural
     * generation. The byte layout of `data` must match the texture @ref format
     * and the size of the updated region.
     *
     * @param mip_level Mip level to update.
     * @param x X offset in pixels.
     * @param y Y offset in pixels.
     * @param w Width of the updated region in pixels.
     * @param h Height of the updated region in pixels.
     * @param data Raw pixel bytes for the updated region.
     */
    auto UpdateSubregion(
        unsigned int mip_level,
        int x,
        int y,
        int w,
        int h,
        std::span<const std::uint8_t> data
    ) -> void;

private:
    /// @cond INTERNAL
    struct PendingUpdate {
        unsigned int mip_level;
        int x;
        int y;
        int w;
        int h;
        std::vector<std::uint8_t> bytes;
    };

    friend class GLTextures;
    std::vector<PendingUpdate> pending_;
    /// @endcond
};

}