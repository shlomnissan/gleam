/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/core/disposable.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace vglx {

/**
 * @brief Represents a named stream of per-vertex or per-instance data.
 *
 * BufferAttribute is the basic unit of mesh data. It owns a flat
 * array of floats along with the metadata needed to interpret it: a @ref name
 * that matches the attribute's name in shader code, a @ref format describing
 * the number of components per element, and a @ref rate that determines
 * whether the data advances per vertex or per instance.
 *
 * Attributes are added to a @ref Geometry (vertex rate) or an
 * @ref InstancedMesh "instanced mesh" (instance rate) and may be shared by
 * multiple geometries. The renderer uploads attribute data to the GPU on
 * first use and re-uploads it whenever the data changes, using an internal
 * version that @ref SetData and @ref Write increment.
 *
 * @code
 * auto positions = vglx::BufferAttribute::Create({
 *   .name = vglx::BufferAttribute::kPosition,
 *   .format = vglx::BufferAttribute::Format::Float32x3,
 *   .rate = vglx::BufferAttribute::Rate::Vertex
 * }, {
 *   0.5f, -0.5f, 0.0f,
 *   0.0f,  0.5f, 0.0f,
 *  -0.5f, -0.5f, 0.0f,
 * });
 *
 * geometry->AddAttribute(positions);
 * @endcode
 *
 * Note that attributes are non-interleaved: each attribute holds a tightly
 * packed array of its own values and is uploaded to its own GPU buffer
 * rather than sharing an interleaved buffer with other attributes.
 *
 * @ingroup GeometryGroup
 */
class VGLX_EXPORT BufferAttribute : public Disposable {
public:
    /**
     * @name Known attribute names
     *
     * Attributes created with these names are picked up automatically by
     * the built-in shaders using the formats listed below. Attributes with
     * any other name are custom: they require a matching declaration in the
     * shader code of a @ref ShaderMaterial. Prefer these constants over
     * spelling the strings out to avoid silent name mismatches.
     */
    ///@{
    static constexpr std::string_view kPosition = "a_Position"; ///< Vertex position in object space (Float32x3).
    static constexpr std::string_view kNormal = "a_Normal"; ///< Vertex normal in object space (Float32x3).
    static constexpr std::string_view kTexCoord = "a_TexCoord"; ///< Texture coordinates (Float32x2).
    static constexpr std::string_view kColor = "a_Color"; ///< Per-vertex color (Float32x3).
    static constexpr std::string_view kTangent = "a_Tangent"; ///< Tangent with handedness in `w` for normal mapping (Float32x4).
    static constexpr std::string_view kInstanceTransform = "a_InstanceTransform"; ///< Per-instance transform matrix (Float32x16, instance rate).
    static constexpr std::string_view kInstanceColor = "a_InstanceColor"; ///< Per-instance color (Float32x3, instance rate).
    ///@}

    /**
     * @brief Enumerates supported attribute data formats.
     *
     * Describes the number of 32-bit float components that make up a single element.
     */
    enum class Format {
        Float32x1, ///< One float per element (scalar).
        Float32x2, ///< Two floats per element (vec2).
        Float32x3, ///< Three floats per element (vec3).
        Float32x4, ///< Four floats per element (vec4).
        Float32x16 ///< Sixteen floats per element (mat4).
    };

    /**
     * @brief Determines how the attribute advances during rendering.
     */
    enum class Rate {
        Vertex, ///< One element per vertex.
        Instance ///< One element per instance.
    };

    /**
     * @brief Parameters for constructing a buffer attribute.
     */
    struct Params {
        std::string_view name {}; ///< Attribute name matching its name in shader code.
        Format format {Format::Float32x1}; ///< Number of components per element.
        Rate rate {Rate::Vertex}; ///< Per-vertex or per-instance rate.
    };

    /// @brief Attribute name matching its name in shader code.
    const std::string name;

    /// @brief Number of float components per element.
    const Format format {Format::Float32x1};

    /// @brief Determines whether the data advances per vertex or per instance.
    const Rate rate {Rate::Vertex};

    /**
     * @brief Constructs a buffer attribute.
     *
     * The data size must be divisible by the number of components implied by
     * the format. Invalid parameters are reported and leave the attribute in
     * an invalid state.
     *
     * @param params Attribute parameters.
     * @param data Flat array of floats containing the attribute data.
     */
    BufferAttribute(const Params& params, std::vector<float> data);

    BufferAttribute(const BufferAttribute&) = delete;
    auto operator=(const BufferAttribute&) -> BufferAttribute& = delete;

    /**
     * @brief Creates a shared instance of @ref BufferAttribute.
     *
     * @param params Attribute parameters.
     * @param data Flat array of floats containing the attribute data.
     */
    [[nodiscard]] static auto Create(const Params& params, std::vector<float> data) -> std::shared_ptr<BufferAttribute> {
        return std::make_shared<BufferAttribute>(params, std::move(data));
    }

    /**
     * @brief Replaces the attribute data.
     *
     * The new data may have a different element count but must be divisible
     * by the number of components implied by the format, otherwise the update
     * is rejected. A successful update marks the attribute for re-upload.
     *
     * @param data Flat array of floats containing the new attribute data.
     */
    auto SetData(std::vector<float> data) -> void;

    /**
     * @brief Overwrites a range of the attribute data in place.
     *
     * The range `[offset, offset + values.size())` must fit within the
     * existing data, otherwise the write is rejected. A successful write
     * marks the attribute for re-upload.
     *
     * @param offset Position of the first float to overwrite.
     * @param values Values to copy into the attribute data.
     */
    auto Write(std::size_t offset, std::span<const float> values) -> void;

    /**
     * @brief Returns the number of float components per element.
     */
    [[nodiscard]] auto Components() const -> uint32_t;

    /**
     * @brief Returns the number of elements stored in the attribute.
     *
     * The element count is derived from the data size and the number of
     * components.
     */
    [[nodiscard]] auto ElementCount() const -> uint32_t;

    /**
     * @brief Checks whether the attribute has a name and data.
     */
    [[nodiscard]] auto IsValid() const -> bool { return !name.empty() && !data_.empty(); }

    /**
     * @brief Returns a read-only reference to the attribute data.
     */
    [[nodiscard]] auto GetData() const -> const std::vector<float>& { return data_; }

    /**
     * @brief Returns the data version, incremented on every update.
     *
     * The renderer compares versions to decide when the attribute needs to be re-uploaded.
     */
    [[nodiscard]] auto GetVersion() const -> uint32_t { return version_; }

private:
    /// @cond INTERNAL
    std::vector<float> data_;

    uint32_t version_ {0};
    /// @endcond
};

}
