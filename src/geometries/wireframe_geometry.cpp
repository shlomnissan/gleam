/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/geometries/wireframe_geometry.hpp"

#include "vglx/math/utilities.hpp"

#include "utilities/logger.hpp"

#include <cstddef>
#include <cstdint>
#include <unordered_set>
#include <utility>
#include <vector>

namespace vglx {

WireframeGeometry::WireframeGeometry(const Geometry* geometry) {
    if (geometry->primitive != Geometry::PrimitiveType::Triangles) {
        Logger::Log(
            LogLevel::Error,
            "Failed to initialize wireframe geometry with non-triangulated source"
        );
    }

    if (geometry->GetIndexData().empty()) {
        Logger::Log(
            LogLevel::Error,
            "Failed to initialize wireframe geometry with non-indexed source"
        );
        return;
    }

    primitive = Geometry::PrimitiveType::Lines;

    for (const auto& attribute : geometry->GetAttributes()) {
        AddAttribute(attribute);
    }

    auto indices = std::vector<uint32_t> {};
    auto seen_edges = std::unordered_set<uint64_t> {};
    auto add_edge = [&indices, &seen_edges](uint32_t x, uint32_t y) {
        const auto key = math::CantorPairingUnordered(
            static_cast<uint64_t>(x),
            static_cast<uint64_t>(y)
        );
        if (seen_edges.emplace(key).second) {
            indices.emplace_back(x);
            indices.emplace_back(y);
        }
    };

    const auto& index_data = geometry->GetIndexData();
    for (auto i = std::size_t {0}; i + 2 < index_data.size(); i += 3) {
        const auto i0 = index_data[i];
        const auto i1 = index_data[i + 1];
        const auto i2 = index_data[i + 2];
        add_edge(i0, i1);
        add_edge(i1, i2);
        add_edge(i2, i0);
    }

    SetIndices(std::move(indices));
}

}
