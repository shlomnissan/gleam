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
#include "vglx/textures/cube_texture.hpp"
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
 * auto material = vglx::PhongMaterial::Create({
 *   .color = 0x049EF4u,
 *   .specular_color = 0x333333u,
 *   .shininess = 64.0f,
 *   .albedo_map = texture
 * });
 *
 * my_scene->Add(vglx::Mesh::Create(geometry, material));
 * @endcode
 *
 * @ingroup MaterialsGroup
 */
class VGLX_EXPORT PhongMaterial : public Material {
public:
    /**
     * @brief Parameters for constructing a @ref PhongMaterial object.
     */
    struct Parameters {
        Color color {0xFFFFFFu}; ///< Diffuse base color.
        Color specular_color {0x111111u}; ///< Specular highlight tint.
        Color emissive_color {0x000000u}; ///< Emissive color independent of lighting.
        float ao_intensity {1.0f}; ///< AO contribution strength.
        float shininess {32.0f}; ///< Specular highlight glossiness.
        float emissive_intensity {1.0f}; ///< Emissive multiplier.
        float normal_intensity {1.0f}; ///< Normal map strength.
        float reflectivity {1.0f}; ///< Environment reflection strength.
        std::shared_ptr<Texture> albedo_map {nullptr}; ///< Diffuse color map.
        std::shared_ptr<Texture> alpha_map {nullptr}; ///< Per-pixel opacity map.
        std::shared_ptr<Texture> ao_map {nullptr}; ///< Ambient occlusion map (R channel).
        std::shared_ptr<Texture> emissive_map {nullptr}; ///< Emissive color map.
        std::shared_ptr<CubeTexture> environment_map {nullptr}; ///< Environment reflection cube map.
        std::shared_ptr<Texture> normal_map {nullptr}; ///< Surface normal map.
        std::shared_ptr<Texture> specular_map {nullptr}; ///< Specular intensity map.
    };

    /// @brief Base surface color used for diffuse reflection.
    Color color;

    /// @brief Specular color controlling the tint of specular highlights.
    Color specular_color;

    /// @brief Emissive color added to the final shaded result, independent of lighting.
    Color emissive_color;

    /// @brief Strength of the ambient occlusion contribution.
    float ao_intensity;

    /// @brief Controls the glossiness of highlights; higher values yield sharper specular peaks.
    float shininess;

    /// @brief Scalar multiplier for emissive contribution.
    float emissive_intensity;

    /// @brief Scalar multiplier for normal map perturbation.
    float normal_intensity;

    /// @brief Strength of the environment map reflection contribution.
    float reflectivity;

    /// @brief Albedo (diffuse) map defining base color and optional alpha channel.
    std::shared_ptr<Texture> albedo_map;

    /// @brief Alpha map defining per-pixel opacity.
    std::shared_ptr<Texture> alpha_map;

    /// @brief Ambient occlusion map sampled from the R channel; modulates the ambient term.
    std::shared_ptr<Texture> ao_map;

    /// @brief Emissive map modulating the emissive color per texel.
    std::shared_ptr<Texture> emissive_map;

    /// @brief Environment cube map sampled for reflection contribution.
    std::shared_ptr<CubeTexture> environment_map;

    /// @brief Normal map for per-pixel surface detail and lighting variation.
    std::shared_ptr<Texture> normal_map;

    /// @brief Specular map scaling the intensity of specular highlights.
    std::shared_ptr<Texture> specular_map;

    /**
     * @brief Constructs a Phong material with default parameters.
     */
    PhongMaterial() : PhongMaterial(Parameters{}) {}

    /**
     * @brief Constructs a Phong material from the given parameters.
     *
     * @param params @ref PhongMaterial::Parameters "Initialization parameters".
     */
    explicit PhongMaterial(const Parameters& params)
      : color(params.color),
        specular_color(params.specular_color),
        emissive_color(params.emissive_color),
        ao_intensity(params.ao_intensity),
        shininess(params.shininess),
        emissive_intensity(params.emissive_intensity),
        normal_intensity(params.normal_intensity),
        reflectivity(params.reflectivity),
        albedo_map(params.albedo_map),
        alpha_map(params.alpha_map),
        ao_map(params.ao_map),
        emissive_map(params.emissive_map),
        environment_map(params.environment_map),
        normal_map(params.normal_map),
        specular_map(params.specular_map) {}

    /**
     * @brief Creates a shared instance of @ref PhongMaterial with default parameters.
     */
    [[nodiscard]] static auto Create() -> std::shared_ptr<PhongMaterial> {
        return std::make_shared<PhongMaterial>();
    }

    /**
     * @brief Creates a shared instance of @ref PhongMaterial.
     *
     * @param params @ref PhongMaterial::Parameters "Initialization parameters".
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::shared_ptr<PhongMaterial> {
        return std::make_shared<PhongMaterial>(params);
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
