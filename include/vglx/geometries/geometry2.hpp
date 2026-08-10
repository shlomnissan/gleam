/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/core/disposable.hpp"
#include "vglx/core/identity.hpp"
#include "vglx/geometries/buffer_attribute.hpp"
#include "vglx/math/box3.hpp"
#include "vglx/math/sphere.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

namespace vglx {

class VGLX_EXPORT Geometry2 : public Disposable, public Identity {
public:
    enum class PrimitiveType {
        Triangles,
        Lines,
        LineLoop
    };

    PrimitiveType primitive { PrimitiveType::Triangles };

    Geometry2() = default;

    [[nodiscard]] static auto Create() -> std::shared_ptr<Geometry2> {
        return std::make_shared<Geometry2>();
    }

    auto AddAttribute(std::shared_ptr<BufferAttribute> attribute) -> void;

    auto SetIndices(std::vector<uint32_t> index_data) -> void;

    [[nodiscard]] auto GetAttributes() const -> const std::vector<std::shared_ptr<BufferAttribute>>& { return attributes_; }

    [[nodiscard]] auto GetAttribute(std::string_view name) const -> std::shared_ptr<BufferAttribute>;

    [[nodiscard]] auto BoundingBox() -> Box3;

    [[nodiscard]] auto BoundingSphere() -> Sphere;

    [[nodiscard]] auto VertexCount() const -> uint32_t;

    [[nodiscard]] auto HasPositions() const -> bool;

    [[nodiscard]] auto GetIndexData() const -> const std::vector<uint32_t>& { return index_data_; }

    [[nodiscard]] auto GetLayoutVersion() const -> uint32_t { return layout_version_; }

    [[nodiscard]] auto GetIndexVersion() const -> uint32_t { return index_version_; }

    [[nodiscard]] auto GetMaxIndex() const -> uint32_t { return max_index_; }

    virtual ~Geometry2();

private:
    std::vector<std::shared_ptr<BufferAttribute>> attributes_ {};

    std::vector<uint32_t> index_data_ {};

    std::optional<std::pair<Box3, uint32_t>> bounding_box_ {};

    std::optional<std::pair<Sphere, uint32_t>> bounding_sphere_ {};

    uint32_t layout_version_ {0};

    uint32_t index_version_ {0};

    uint32_t max_index_ {0};
};

}
