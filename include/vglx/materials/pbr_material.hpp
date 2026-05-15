/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/materials/material.hpp"
#include "vglx/math/color.hpp"
#include "vglx/textures/texture_2d.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Implements physically-based shading using the metallic-roughness workflow.
 *
 * This material implements [physically-based rendering](https://en.wikipedia.org/wiki/Physically_based_rendering)
 * with a Cook-Torrance specular term and Lambertian diffuse, parameterized
 * by metallic and roughness factors. It is intended for realistic lit
 * geometry from rough dielectrics such as wood or plastic to
 * polished metals.
 *
 * @code
 * auto material = vglx::PBRMaterial::Create(0xFFFFFF);
 * material->metallic = 1.0f;
 * material->roughness = 0.4f;
 * material->albedo_map = texture;
 *
 * my_scene->Add(vglx::Mesh::Create(geometry, material));
 * @endcode
 *
 * @ingroup MaterialsGroup
 */
class VGLX_EXPORT PBRMaterial : public Material {
public:
    /// @brief Base surface color; for metals this acts as the specular tint.
    Color color = 0xFFFFFF;

    /// @brief Emissive color added to the final shaded result, independent of lighting.
    Color emissive_color = 0x000000;

    /// @brief Strength of the ambient occlusion contribution; 0 disables, 1 applies fully.
    float ao_intensity = 1.0f;

    /// @brief Scalar multiplier for emissive contribution.
    float emissive_intensity = 1.0f;

    /// @brief Metallic factor; 0 for dielectrics, 1 for metals.
    float metallic = 0.0f;

    /// @brief Scalar multiplier for normal map perturbation; 0 disables, 1 applies fully.
    float normal_intensity = 1.0f;

    /// @brief Roughness factor; 0 produces sharp mirror reflections, 1 yields a fully rough surface.
    float roughness = 1.0f;

    /// @brief Alpha map defining per-pixel opacity.
    std::shared_ptr<Texture> alpha_map = nullptr;

    /// @brief Ambient occlusion map sampled from the R channel; modulates the ambient term.
    std::shared_ptr<Texture> ao_map = nullptr;

    /// @brief Albedo (base color) map multiplied with @ref color per texel.
    std::shared_ptr<Texture> albedo_map = nullptr;

    /// @brief Emissive map modulating the emissive color per texel.
    std::shared_ptr<Texture> emissive_map = nullptr;

    /// @brief Metallic map sampled from the B channel; multiplied with @ref metallic per texel.
    std::shared_ptr<Texture> metallic_map = nullptr;

    /// @brief Normal map for per-pixel surface detail and lighting variation.
    std::shared_ptr<Texture> normal_map = nullptr;

    /// @brief Roughness map sampled from the G channel; multiplied with @ref roughness per texel.
    std::shared_ptr<Texture> roughness_map = nullptr;

    /**
     * @brief Constructs a PBR material with a given base color.
     *
     * @param color Base color of the material.
     */
    explicit PBRMaterial(const Color& color) : color(color) {}

    /**
     * @brief Creates a shared instance of @ref PBRMaterial.
     *
     * @param color Base color of the material.
     */
    [[nodiscard]] static auto Create(const Color& color = 0xFFFFFF) -> std::shared_ptr<PBRMaterial> {
        return std::make_shared<PBRMaterial>(color);
    }

    /**
     * @brief Identifies this material as
     * @ref Material::Type "Material::Type::PBRMaterial".
     */
    auto GetType() const -> Type override {
        return Material::Type::PBRMaterial;
    }
};

}
