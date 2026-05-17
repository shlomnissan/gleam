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
 * @brief Implements the Blinn–Phong shading model for glossy surfaces.
 *
 * This material implements a classic [Blinn–Phong lighting model](https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_reflection_model)
 * with diffuse and specular reflection. It is intended for standard
 * lit geometry that requires view-dependent highlights, such as polished
 * surfaces or simple metals.
 *
 * @code
 * auto material = vglx::PhongMaterial::Create(0x049EF4);
 * material->specular = 0x333333;
 * material->shininess = 64.0f;
 * material->albedo_map = texture;
 *
 * my_scene->Add(vglx::Mesh::Create(geometry, material));
 * @endcode
 *
 * @ingroup MaterialsGroup
 */
class VGLX_EXPORT PhongMaterial : public Material {
public:
    /// @brief Base surface color used for diffuse reflection.
    Color color = 0xFFFFFF;

    /// @brief Specular color controlling the tint of specular highlights.
    Color specular_color = 0x111111;

    /// @brief Emissive color added to the final shaded result, independent of lighting.
    Color emissive_color = 0x000000;

    /// @brief Strength of the ambient occlusion contribution; 0 disables, 1 applies fully.
    float ao_intensity = 1.0f;

    /// @brief Controls the glossiness of highlights; higher values yield sharper specular peaks.
    float shininess = 32.0f;

    /// @brief Scalar multiplier for emissive contribution.
    float emissive_intensity = 1.0f;

    /// @brief Scalar multiplier for normal map perturbation; 0 disables, 1 applies fully.
    float normal_intensity = 1.0f;

    /// @brief Albedo (diffuse) map defining base color and optional alpha channel.
    std::shared_ptr<Texture> albedo_map = nullptr;

    /// @brief Alpha map defining per-pixel opacity.
    std::shared_ptr<Texture> alpha_map = nullptr;

    /// @brief Ambient occlusion map sampled from the R channel; modulates the ambient term.
    std::shared_ptr<Texture> ao_map = nullptr;

    /// @brief Emissive map modulating the emissive color per texel.
    std::shared_ptr<Texture> emissive_map = nullptr;

    /// @brief Normal map for per-pixel surface detail and lighting variation.
    std::shared_ptr<Texture> normal_map = nullptr;

    /// @brief Specular map scaling the intensity of specular highlights.
    std::shared_ptr<Texture> specular_map = nullptr;

    /**
     * @brief Constructs a Phong material with a given base color.
     *
     * @param color Base diffuse color of the material.
     */
    explicit PhongMaterial(const Color& color) : color(color) {}

    /**
     * @brief Creates a shared instance of @ref PhongMaterial.
     *
     * @param color Base diffuse color of the material.
     */
    [[nodiscard]] static auto Create(const Color& color = 0xFFFFFF) -> std::shared_ptr<PhongMaterial> {
        return std::make_shared<PhongMaterial>(color);
    }

    /**
     * @brief Identifies this material as
     * @ref Material::Type "Material::Type::PhongMaterial".
     */
    auto GetType() const -> Type override {
        return Material::Type::PhongMaterial;
    }
};

}