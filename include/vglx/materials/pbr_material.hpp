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
 * auto material = vglx::PBRMaterial::Create({
 *   .color = 0xFFFFFF,
 *   .metallic = 1.0f,
 *   .roughness = 0.4f,
 *   .albedo_map = texture
 * });
 *
 * my_scene->Add(vglx::Mesh::Create(geometry, material));
 * @endcode
 *
 * @ingroup MaterialsGroup
 */
class VGLX_EXPORT PBRMaterial : public Material {
public:
    /**
     * @brief Parameters for constructing a @ref PBRMaterial object.
     */
    struct Parameters {
        Color color = 0xFFFFFF; ///< Base surface color.
        Color emissive_color = 0x000000; ///< Emissive color independent of lighting.
        float ao_intensity = 1.0f; ///< AO contribution strength.
        float emissive_intensity = 1.0f; ///< Emissive multiplier.
        float environment_intensity = 1.0f; ///< Environment lighting multiplier.
        float metallic = 0.0f; ///< Metallic factor.
        float normal_intensity = 1.0f; ///< Normal map strength.
        float roughness = 1.0f; ///< Roughness factor.
        std::shared_ptr<Texture> alpha_map = nullptr; ///< Per-pixel opacity map.
        std::shared_ptr<Texture> ao_map = nullptr; ///< Ambient occlusion map (R channel).
        std::shared_ptr<Texture> albedo_map = nullptr; ///< Base color map.
        std::shared_ptr<Texture> emissive_map = nullptr; ///< Emissive color map.
        std::shared_ptr<Texture> metallic_map = nullptr; ///< Metallic map (B channel).
        std::shared_ptr<Texture> normal_map = nullptr; ///< Surface normal map.
        std::shared_ptr<Texture> roughness_map = nullptr; ///< Roughness map (G channel).
    };

    /// @brief Base surface color; for metals this acts as the specular tint.
    Color color;

    /// @brief Emissive color added to the final shaded result, independent of lighting.
    Color emissive_color;

    /// @brief Strength of the ambient occlusion contribution.
    float ao_intensity;

    /// @brief Scalar multiplier for emissive contribution.
    float emissive_intensity;

    /// @brief Scalar multiplier for environment lighting. Multiplies @ref Scene::environment_intensity.
    float environment_intensity;

    /// @brief Metallic factor.
    float metallic;

    /// @brief Scalar multiplier for normal map perturbation.
    float normal_intensity;

    /// @brief Roughness factor.
    float roughness;

    /// @brief Alpha map defining per-pixel opacity.
    std::shared_ptr<Texture> alpha_map;

    /// @brief Ambient occlusion map that modulates the ambient term.
    std::shared_ptr<Texture> ao_map;

    /// @brief Albedo (base color) map multiplied with @ref color per texel.
    std::shared_ptr<Texture> albedo_map;

    /// @brief Emissive map modulating the emissive color per texel.
    std::shared_ptr<Texture> emissive_map;

    /// @brief Metallic map sampled from the B channel; multiplied with @ref metallic per texel.
    std::shared_ptr<Texture> metallic_map;

    /// @brief Normal map for per-pixel surface detail and lighting variation.
    std::shared_ptr<Texture> normal_map;

    /// @brief Roughness map sampled from the G channel; multiplied with @ref roughness per texel.
    std::shared_ptr<Texture> roughness_map;

    /**
     * @brief Constructs a PBR material with default parameters.
     */
    PBRMaterial() : PBRMaterial(Parameters{}) {}

    /**
     * @brief Constructs a PBR material from the given parameters.
     *
     * @param params @ref PBRMaterial::Parameters "Initialization parameters".
     */
    explicit PBRMaterial(const Parameters& params)
      : color(params.color),
        emissive_color(params.emissive_color),
        ao_intensity(params.ao_intensity),
        emissive_intensity(params.emissive_intensity),
        environment_intensity(params.environment_intensity),
        metallic(params.metallic),
        normal_intensity(params.normal_intensity),
        roughness(params.roughness),
        alpha_map(params.alpha_map),
        ao_map(params.ao_map),
        albedo_map(params.albedo_map),
        emissive_map(params.emissive_map),
        metallic_map(params.metallic_map),
        normal_map(params.normal_map),
        roughness_map(params.roughness_map) {}

    /**
     * @brief Creates a shared instance of @ref PBRMaterial with default parameters.
     */
    [[nodiscard]] static auto Create() -> std::shared_ptr<PBRMaterial> {
        return std::make_shared<PBRMaterial>();
    }

    /**
     * @brief Creates a shared instance of @ref PBRMaterial.
     *
     * @param params @ref PBRMaterial::Parameters "Initialization parameters".
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::shared_ptr<PBRMaterial> {
        return std::make_shared<PBRMaterial>(params);
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
