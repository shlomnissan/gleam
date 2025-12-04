/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/geometries/wireframe_geometry.hpp"

#include "vglx/math/utilities.hpp"

#include "utilities/logger.hpp"

#include <cinttypes>
#include <unordered_set>

namespace vglx {

WireframeGeometry::WireframeGeometry(const Geometry* geometry) :
    Geometry(geometry->VertexData(), {})
{
    if (geometry->primitive != Geometry::PrimitiveType::Triangles) {
        Logger::Log(
            LogLevel::Error,
            "Failed to initialize wireframe geometry with non-triangulated source"
        );
    }

    if (geometry->IndexCount() == 0) {
        Logger::Log(
            LogLevel::Error,
            "Failed to initialize wireframe geometry with non-indexed source"
        );
        return;
    }

    primitive = Geometry::PrimitiveType::Lines;
    attributes_ = geometry->Attributes();

    auto seen_edges = std::unordered_set<uint64_t> {};
    auto add_edge = [this, &seen_edges](unsigned int x, unsigned int y) {
        const auto key = math::CantorPairingUnordered(
            static_cast<uint64_t>(x),
            static_cast<uint64_t>(y)
        );
        if (seen_edges.emplace(key).second) {
            index_data_.emplace_back(x);
            index_data_.emplace_back(y);
        }
    };

    for (auto i = 0; i < geometry->IndexCount(); i += 3) {
        const auto i0 = geometry->IndexData()[i];
        const auto i1 = geometry->IndexData()[i + 1];
        const auto i2 = geometry->IndexData()[i + 2];
        add_edge(i0, i1);
        add_edge(i1, i2);
        add_edge(i2, i0);
    }
}

}