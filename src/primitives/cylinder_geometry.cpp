/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/primitives/cylinder_geometry.hpp"

#include "vglx/math/utilities.hpp"
#include "vglx/math/vector3.hpp"

#include "geometries/vertex_streams.hpp"

#include <cassert>
#include <cstdint>

namespace vglx {

namespace {

auto generate_torso(
    const CylinderGeometry::Parameters& params,
    VertexStreams& streams
) {
    const auto half_height = params.height / 2;
    const auto slope = (params.radius_bottom - params.radius_top) / params.height;

    for (auto y = 0; y <= params.height_segments; ++y) {
        const auto v = static_cast<float>(y) / static_cast<float>(params.height_segments);
        const auto radius = params.radius_top + (params.radius_bottom - params.radius_top) * v;

        for (auto x = 0; x <= params.radial_segments; ++x) {
            const auto u = static_cast<float>(x) / static_cast<float>(params.radial_segments);
            const auto theta = u * math::two_pi;
            const auto cos_theta = math::Cos(theta);
            const auto sin_theta = math::Sin(theta);
            const auto normal = Vector3(sin_theta, slope, cos_theta).Normalize();

            streams.positions.emplace_back(radius * sin_theta);
            streams.positions.emplace_back(-v * params.height + half_height);
            streams.positions.emplace_back(radius * cos_theta);

            streams.normals.emplace_back(normal.x);
            streams.normals.emplace_back(normal.y);
            streams.normals.emplace_back(normal.z);

            streams.uvs.emplace_back(u);
            streams.uvs.emplace_back(1.0f - v);
        }
    }

    for (auto y = 0; y < params.height_segments; ++y) {
        for (auto x = 0; x < params.radial_segments; ++x) {
            const auto a = x + (params.radial_segments + 1) * y;
            const auto b = x + (params.radial_segments + 1) * (y + 1);
            const auto c = x + 1 + (params.radial_segments + 1) * (y + 1);
            const auto d = x + 1 + (params.radial_segments + 1) * y;

            streams.indices.emplace_back(a);
            streams.indices.emplace_back(b);
            streams.indices.emplace_back(d);
            streams.indices.emplace_back(b);
            streams.indices.emplace_back(c);
            streams.indices.emplace_back(d);
        }
    }
}

auto generate_cap(
    const CylinderGeometry::Parameters& params,
    bool top,
    VertexStreams& streams
) {
    const auto half_height = params.height / 2;
    const auto sign = top ? 1.0f : -1.0f;
    const auto center_index_start = streams.positions.size() / 3;

    // Generate the center of the cap (multiple vertices at the same position)
    for (auto x = 0; x < params.radial_segments; ++x) {
        streams.positions.emplace_back(0.0f);
        streams.positions.emplace_back(half_height * sign);
        streams.positions.emplace_back(0.0f);

        streams.normals.emplace_back(0.0f);
        streams.normals.emplace_back(sign);
        streams.normals.emplace_back(0.0f);

        streams.uvs.emplace_back(0.5f);
        streams.uvs.emplace_back(0.5f);
    }

    const auto radius = top ? params.radius_top : params.radius_bottom;
    const auto center_index_end = streams.positions.size() / 3;

    // Generate the vertices around the center
    for (auto x = 0; x <= params.radial_segments; ++x) {
        const auto u = static_cast<float>(x) / static_cast<float>(params.radial_segments);
        const auto theta = u * math::two_pi;
        const auto cos_theta = math::Cos(theta);
        const auto sin_theta = math::Sin(theta);

        streams.positions.emplace_back(radius * sin_theta);
        streams.positions.emplace_back(half_height * sign);
        streams.positions.emplace_back(radius * cos_theta);

        streams.normals.emplace_back(0.0f);
        streams.normals.emplace_back(sign);
        streams.normals.emplace_back(0.0f);

        streams.uvs.emplace_back(cos_theta * 0.5f + 0.5f);
        streams.uvs.emplace_back(sin_theta * 0.5f * sign + 0.5f);
    }

    for (auto x = 0; x < params.radial_segments; ++x) {
        const auto c = static_cast<uint32_t>(center_index_start + x);
        const auto i = static_cast<uint32_t>(center_index_end + x);

        if (top) {
            streams.indices.emplace_back(i);
            streams.indices.emplace_back(i + 1);
            streams.indices.emplace_back(c);
        } else {
            streams.indices.emplace_back(i + 1);
            streams.indices.emplace_back(i);
            streams.indices.emplace_back(c);
        }
    }
}

}

CylinderGeometry::CylinderGeometry(const Parameters& params) {
    assert(params.height > 0.0f);
    assert(params.radial_segments > 0);
    assert(params.height_segments > 0);

    SetName("cylinder geometry");

    auto streams = VertexStreams {};

    generate_torso(params, streams);
    if (!params.open_ended) {
        if (params.radius_top > 0.0f) generate_cap(params, true, streams);
        if (params.radius_bottom > 0.0f) generate_cap(params, false, streams);
    }

    streams.AddTo(*this);
}

}
