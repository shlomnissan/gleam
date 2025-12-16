/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/primitives/plane_geometry.hpp"

#include <cassert>

namespace vglx {

namespace {

auto generate_geometry(
    const PlaneGeometry::Parameters& params,
    std::vector<float>& vertex_data,
    std::vector<unsigned int>& index_data
) {
    const auto width_half = params.width / 2;
    const auto height_half = params.height / 2;

    const auto grid_x = params.width_segments;
    const auto grid_y = params.height_segments;
    const auto grid_x1 = grid_x + 1;
    const auto grid_y1 = grid_y + 1;

    const auto segment_w = params.width / grid_x;
    const auto segment_h = params.height / grid_y;

    for (auto iy = 0; iy < grid_y1; ++iy) {
        const auto y = iy * segment_h - height_half;
        for (auto ix = 0; ix < grid_x1; ++ix) {
            const auto x = ix * segment_w - width_half;
            const auto u = static_cast<float>(ix) / grid_x;
            const auto v = 1 - (static_cast<float>(iy) / grid_y);

            vertex_data.emplace_back(x); // pos x
            vertex_data.emplace_back(-y); // pos y
            vertex_data.emplace_back(0.0f); // pos z
            vertex_data.emplace_back(0.0f); // normal x
            vertex_data.emplace_back(0.0f); // normal y
            vertex_data.emplace_back(1.0f); // normal z
            vertex_data.emplace_back(u); // u
            vertex_data.emplace_back(v); // v
        }
    }

    for (auto iy = 0; iy < grid_y; ++iy) {
        for (auto ix = 0; ix < grid_x; ++ix) {
            const auto a = ix + grid_x1 * iy;
            const auto b = ix + grid_x1 * (iy + 1);
            const auto c = ix + 1 + grid_x1 * (iy + 1);
            const auto d = ix + 1 + grid_x1 * iy;

            index_data.emplace_back(a);
            index_data.emplace_back(b);
            index_data.emplace_back(d);
            index_data.emplace_back(b);
            index_data.emplace_back(c);
            index_data.emplace_back(d);
        }
    }
}

}

PlaneGeometry::PlaneGeometry(const Parameters& params) {
    assert(params.width > 0.0f);
    assert(params.height > 0.0f);
    assert(params.width_segments > 0);
    assert(params.height_segments > 0);

    SetName("plane geometry");

    generate_geometry(params, vertex_data_, index_data_);

    SetAttribute({.type = Geometry::VertexAttributeType::Position, .item_size = 3});
    SetAttribute({.type = Geometry::VertexAttributeType::Normal, .item_size = 3});
    SetAttribute({.type = Geometry::VertexAttributeType::UV, .item_size = 2});
}

}