/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/textures/image.hpp"
#include "vglx/textures/texture.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Represents a cube map texture.
 *
 * A cube texture stores six face images that form a cube map, commonly used
 * for skyboxes and environment mapping. Each face corresponds to a direction
 * along the positive or negative X, Y, or Z axis.
 *
 * @ingroup TexturesGroup
 */
class VGLX_EXPORT CubeTexture : public Texture {
public:
    /**
     * @brief The six face images that make up the cube map.
     */
    struct Images {
        std::shared_ptr<Image> positiveX; ///< Right face (+X).
        std::shared_ptr<Image> negativeX; ///< Left face (-X).
        std::shared_ptr<Image> positiveY; ///< Top face (+Y).
        std::shared_ptr<Image> negativeY; ///< Bottom face (-Y).
        std::shared_ptr<Image> positiveZ; ///< Front face (+Z).
        std::shared_ptr<Image> negativeZ; ///< Back face (-Z).
    };

    /// @brief The source images backing this cube map.
    Images images;

    /**
     * @brief Constructs a cube texture from six face @ref Image "images".
     *
     * The texture holds shared references to each face image, allowing
     * multiple textures to share the same underlying pixel data.
     *
     * @param images The six face images that make up the cube map.
     */
    explicit CubeTexture(Images images) : images(std::move(images)) {}

    /**
     * @brief Creates a shared instance of @ref CubeTexture.
     *
     * @param images The six face images that make up the cube map.
     */
    [[nodiscard]] static auto Create(Images images) -> std::shared_ptr<CubeTexture> {
        return std::make_shared<CubeTexture>(std::move(images));
    }

    /**
     * @brief Identifies this texture as @ref Texture::Type "Texture::Type::CubeTexture".
     */
    [[nodiscard]] auto GetType() const -> Texture::Type override {
        return Texture::Type::CubeTexture;
    }
};

}
