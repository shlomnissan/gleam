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
#include "vglx/math/matrix3.hpp"
#include "vglx/math/matrix4.hpp"
#include "vglx/math/vector2.hpp"
#include "vglx/math/vector3.hpp"
#include "vglx/math/vector4.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

namespace vglx {

/**
 * @brief Material rendered using user-defined GLSL shaders.
 *
 * Shader material provides full control over the shading pipeline by allowing
 * custom vertex and fragment shader code. It is intended for advanced
 * rendering effects, experimental lighting models, and post-processing passes
 * that go beyond built-in material types.
 *
 * Uniforms can be defined dynamically at creation time and later updated by
 * name. Supported uniform types include scalar, vector, matrix, and color
 * values.
 *
 * @code
 * auto material = vglx::ShaderMaterial::Create({
 *   .vertex_shader = vert_source,
 *   .fragment_shader = frag_source,
 *   .uniforms = {
 *     {"u_Time", 0.0f},
 *     {"u_Resolution", Vector2::Zero()}
 *   }
 * });
 *
 * // Update a uniform each frame
 * material->uniforms["u_Time"] = timer.GetElapsedSeconds();
 *
 * my_scene->Add(vglx::Mesh::Create(geometry, material));
 * @endcode
 *
 * @ingroup MaterialsGroup
 */
class VGLX_EXPORT ShaderMaterial : public Material {
public:
    friend class ProgramAttributes;

    /**
     * @brief Represents a supported uniform value type.
     *
     * A uniform can be an integer, float, color, or a vector/matrix type.
     * Values are matched by name when uploaded to the shader program.
     */
    using UniformValue = std::variant<int, float, Color, Matrix3, Matrix4, Vector2, Vector3, Vector4>;

    /**
     * @brief Parameters for constructing a @ref ShaderMaterial object.
     */
    struct Parameters {
        std::string vertex_shader; ///< Vertex shader code.
        std::string fragment_shader; ///< Fragment shader code.
        std::unordered_map<std::string, UniformValue> uniforms; ///< Initial uniform values.
    };

    /**
     * @brief Map of uniform names to their current values.
     *
     * To update a uniform, modify this map directly using the uniform name.
     * Updates take effect on the next frame when the material is bound.
     */
    std::unordered_map<std::string, UniformValue> uniforms;

    /**
     * @brief Constructs a shader material from custom GLSL source strings.
     *
     * @param params @ref ShaderMaterial::Parameters "Initialization parameters"
     * defining the shader sources and initial uniform values.
     */
    ShaderMaterial(const Parameters& params) :
        vertex_shader_(params.vertex_shader),
        fragment_shader_(params.fragment_shader),
        uniforms(params.uniforms) {}

    /**
     * @brief Creates a shared instance of @ref ShaderMaterial.
     *
     * @param params @ref ShaderMaterial::Parameters "Initialization parameters"
     * defining the shader sources and initial uniform values.
     */
    [[nodiscard]] static auto Create(const Parameters& params) -> std::shared_ptr<ShaderMaterial> {
        return std::make_shared<ShaderMaterial>(params);
    }

    /**
     * @brief Identifies this material as
     * @ref Material::Type "Material::Type::ShaderMaterial".
     */
    auto GetType() const -> Type override {
        return Material::Type::ShaderMaterial;
    }

private:
    /// @cond INTERNAL
    std::string vertex_shader_;
    std::string fragment_shader_;
    /// @endcond
};

}