/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

namespace vglx {

/**
 * @brief Per-light shadow configuration.
 *
 * Describes how a light's shadow map is rendered and sampled. This is a plain,
 * user-tunable data owned by the light and exposed as a public member. It
 * holds no GPU resources: the depth framebuffer, depth texture, and computed
 * light-space view-projection are owned by the renderer.
 *
 * Not all fields apply to every light type. The projection kind is determined by
 * the light itself. Fields that don't apply to a given type are ignored.
 *
 * @ingroup LightsGroup
 */
struct Shadow {
    /// @brief Constant depth bias applied during the shadow compare to reduce
    /// self-shadowing artifacts.
    float bias {0.0f};

    /// @brief Resolutio of the square shadow map. Higher values sharpen shadow
    /// edges at the cost of memory and fill rate. Powers of two are recommended.
    int map_size {1024};

    /// @brief Near plane of the light's shadow projection.
    float near_plane {0.5f};

    /// @brief Far plane of the light's shadow projection.
    float far_plane {500.0f};

    /// @brief Orthographic half-extent of the shadow projection, in world units.
    /// Directional lights only. Defines the side length of the light-space box
    /// that receives shadows.
    float extent {10.0f};
};

}