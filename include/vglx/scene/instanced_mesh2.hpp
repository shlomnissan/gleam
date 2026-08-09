/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/geometries/buffer_attribute.hpp"
#include "vglx/math/box3.hpp"
#include "vglx/math/color.hpp"
#include "vglx/math/matrix4.hpp"
#include "vglx/math/sphere.hpp"
#include "vglx/scene/mesh.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

namespace vglx {

class VGLX_EXPORT InstancedMesh2 : public Mesh {
public:
    InstancedMesh2(
        std::shared_ptr<Geometry> geometry,
        std::shared_ptr<Material> material,
        std::size_t count
    );

    [[nodiscard]] static auto Create(
        std::shared_ptr<Geometry> geometry,
        std::shared_ptr<Material> material,
        std::size_t count
    ) -> std::unique_ptr<InstancedMesh2> {
        return std::make_unique<InstancedMesh2>(geometry, material, count);
    }

    [[nodiscard]] auto GetNodeType() const -> Node::Type override {
        return Node::Type::InstancedMesh;
    }

    auto AddInstanceAttribute(std::shared_ptr<BufferAttribute> attribute) -> void;

    [[nodiscard]] auto GetInstanceAttributes() const -> const std::vector<std::shared_ptr<BufferAttribute>>& { return attributes_; }

    [[nodiscard]] auto GetInstanceAttribute(std::string_view name) const -> std::shared_ptr<BufferAttribute>;

    [[nodiscard]] auto GetCount() const -> size_t { return count_; }

    [[nodiscard]] auto GetLayoutVersion() const -> uint32_t { return layout_version_; }

    [[nodiscard]] auto TransformAt(std::size_t idx) const -> Matrix4;

    [[nodiscard]] auto ColorAt(std::size_t idx) const -> Color;

    auto SetTransformAt(std::size_t idx, const Matrix4& matrix) -> void;

    auto SetColorAt(std::size_t idx, const Color& color) -> void;

    auto BoundingBox() -> Box3 override;

    auto BoundingSphere() -> Sphere override;

private:
    std::vector<std::shared_ptr<BufferAttribute>> attributes_ {};

    size_t count_;

    uint32_t layout_version_ {0};

    std::optional<std::pair<Box3, uint32_t>> bounding_box_ {};

    std::optional<std::pair<Sphere, uint32_t>> bounding_sphere_ {};
};

}
