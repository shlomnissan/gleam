/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/primitives/plane_geometry.hpp"

#include "geometries/vertex_streams.hpp"

#include <cassert>

namespace vglx {

namespace {

auto generate_geometry(
    const PlaneGeometry::Parameters& params,
    VertexStreams& streams
) {
    using enum PlaneGeometry::Orientation;

    const auto width_half = params.width / 2;
    const auto height_half = params.height / 2;

    const auto grid_x = params.width_segments;
    const auto grid_y = params.height_segments;
    const auto grid_x1 = grid_x + 1;
    const auto grid_y1 = grid_y + 1;

    const auto segment_w = params.width / grid_x;
    const auto segment_h = params.height / grid_y;

    for (auto iy = 0u; iy < grid_y1; ++iy) {
        const auto pos_h = iy * segment_h - height_half;
        for (auto ix = 0u; ix < grid_x1; ++ix) {
            const auto pos_w = ix * segment_w - width_half;
            const auto u = static_cast<float>(ix) / grid_x;
            const auto v = 1 - (static_cast<float>(iy) / grid_y);
            switch (params.orientation) {
                case FaceX:
                    streams.positions.insert(streams.positions.end(), {0.0f, -pos_h, -pos_w});
                    streams.normals.insert(streams.normals.end(), {1.0f, 0.0f, 0.0f});
                    break;
                case FaceY:
                    streams.positions.insert(streams.positions.end(), {pos_w, 0.0f, pos_h});
                    streams.normals.insert(streams.normals.end(), {0.0f, 1.0f, 0.0f});
                    break;
                case FaceZ:
                    streams.positions.insert(streams.positions.end(), {pos_w, -pos_h, 0.0f});
                    streams.normals.insert(streams.normals.end(), {0.0f, 0.0f, 1.0f});
                    break;
            }

            streams.uvs.emplace_back(u);
            streams.uvs.emplace_back(v);
        }
    }

    for (auto iy = 0u; iy < grid_y; ++iy) {
        for (auto ix = 0u; ix < grid_x; ++ix) {
            const auto a = ix + grid_x1 * iy;
            const auto b = ix + grid_x1 * (iy + 1);
            const auto c = ix + 1 + grid_x1 * (iy + 1);
            const auto d = ix + 1 + grid_x1 * iy;

            streams.indices.emplace_back(a);
            streams.indices.emplace_back(b);
            streams.indices.emplace_back(d);
            streams.indices.emplace_back(b);
            streams.indices.emplace_back(c);
            streams.indices.emplace_back(d);
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

    auto streams = VertexStreams {};
    generate_geometry(params, streams);
    streams.AddTo(*this);
}

}
