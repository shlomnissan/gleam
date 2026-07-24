/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/core/identity.hpp"

#include <string>

namespace vglx {

/**
 * @brief Abstract base class for material types.
 *
 * This class is not intended to be used directly. Use one of the concrete
 * material types such as @ref PhongMaterial, @ref UnlitMaterial,
 * @ref ShaderMaterial, or derive your own material implementation from
 * this class.
 *
 * @ingroup MaterialsGroup
 */
class VGLX_EXPORT Material : public Identity {
public:
    /**
     * @brief Enumerates all supported material types.
     */
    enum class Type {
        PBRMaterial, ///< Implements physically-based rendering with the metallic workflow.
        PhongMaterial, ///< Implements the Blinn–Phong lighting model.
        ShaderMaterial, ///< Uses a custom shader program for rendering.
        SpriteMaterial, ///< Specialized material for 2D sprites and billboards.
        UnlitMaterial, ///< Renders without lighting; color appears as-is.
        Length, ///< Sentinel value representing the number of material types.
    };

    /**
     * @brief Enumerates blending modes used for compositing this material.
     */
    enum class Blending {
        None, ///< No blending (default) fragments overwrite existing pixels.
        Normal, ///< Standard alpha blending.
        Additive, ///< Adds fragment color to the framebuffer color.
        Subtractive, ///< Subtracts fragment color from the framebuffer color.
        Multiply ///< Multiplies fragment color by the framebuffer color.
    };

    /**
     * @brief Enumerates which side of each polygon is rendered.
     */
    enum class Side {
        Front, ///< Renders front faces only (default).
        Back, ///< Renders back faces only.
        TwoSided ///< Renders both front and back faces.
    };

    /**
     * @brief Enumerates depth comparison functions used for depth testing.
     */
    enum class Depth {
        Less, ///< Passes if the fragment depth is less than the stored depth.
        Equal, ///< Passes if the fragment depth is equal to the stored depth.
        LessEqual, ///< Passes if the fragment depth is less than or equal to the stored depth.
        Greater, ///< Passes if the fragment depth is greater than the stored depth.
        GreaterEqual, ///< Passes if the fragment depth is greater than or equal to the stored depth.
        Always, ///< Always passes regardless of depth value.
        Never ///< Never passes regardless of depth value.
    };

    /**
     * @brief Value in the range $[0.0, 1.0]$ indicating how transparent the material is.
     *
     * Blends the material with the framebuffer when @ref Material::transparent
     * "transparency" is enabled, and contributes to the alpha value tested
     * against @ref alpha_test. Has no effect when both are disabled.
     */
    float opacity {1.0f};

    /// @brief Polygon offset factor used to mitigate z-fighting.
    float polygon_offset_factor {0.0f};

    /// @brief Polygon offset units used to mitigate z-fighting.
    float polygon_offset_units {0.0f};

    /**
     * @brief Alpha threshold below which fragments are discarded.
     *
     * Fragments whose final alpha (@ref opacity combined with texture alpha)
     * falls below this value are not rendered. Use for cutout surfaces such
     * as foliage. Unlike @ref transparent, discarded fragments write no depth
     * and require no sorting. A value of 0 disables the test.
     */
    float alpha_test {0.0f};

    /// @brief Enables scene fog for this material.
    bool fog {true};

    /// @brief Enables depth testing.
    bool depth_test {true};

    /// @brief Enables wireframe rendering.
    bool wireframe {false};

    /// @brief Enables transparency for this material.
    bool transparent {false};

    /// @brief Enables flat shading.
    bool flat_shaded {false};

    /// @brief Controls whether this material is visible when rendering.
    bool visible {true};

    /// @brief @ref Side of each polygon rendered by this material.
    Side side {Side::Front};

    /// @brief @ref Blending mode used when rendering this material.
    Blending blending {Blending::Normal};

    /// @brief @ref Depth function used for depth testing.
    Depth depth {Depth::LessEqual};

    /**
     * @brief Identifies the concrete @ref Material::Type "Material::Type" of this material.
     */
    [[nodiscard]] virtual auto GetType() const -> Type = 0;

    /**
     * @brief Converts a material @ref Material::Type "type" value to a string.
     *
     * Primarily intended for debugging and logging.
     *
     * @param type Material type enum value.
     */
    [[nodiscard]] inline static auto TypeToString(Type type) -> std::string {
        switch(type) {
            case Material::Type::PBRMaterial:
                return "pbr_material";
            case Material::Type::PhongMaterial:
                return "phong_material";
            case Material::Type::ShaderMaterial:
                return "shader_material";
            case Material::Type::SpriteMaterial:
                return "sprite_material";
            case Material::Type::UnlitMaterial:
                return "unlit_material";
            default:
                return "unkonwn";
        }
    }

    virtual ~Material() = default;
};

}
