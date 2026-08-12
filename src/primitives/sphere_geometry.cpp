/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/primitives/sphere_geometry.hpp"

#include "vglx/math/utilities.hpp"
#include "vglx/math/vector3.hpp"

#include "geometries/vertex_streams.hpp"

#include <cassert>

namespace vglx {

namespace {

auto generate_geometry(
    const SphereGeometry::Parameters& params,
    VertexStreams& streams
) {
    const auto phi_start = 0.0f;
    const auto phi_length = math::two_pi;
    const auto theta_start = 0.0f;
    const auto theta_length = math::pi;

    for (auto iy = 0; iy <= params.height_segments; ++iy) {
        const auto v = static_cast<float>(iy) / static_cast<float>(params.height_segments);
        for (auto ix = 0; ix <= params.width_segments; ++ix) {
            const auto u = static_cast<float>(ix) / static_cast<float>(params.width_segments);
            const auto phi = phi_start + u * phi_length;
            const auto theta = theta_start + v * theta_length;

            auto vec = Vector3 {};
            vec.x = -params.radius * math::Cos(phi) * math::Sin(theta);
            vec.y =  params.radius * math::Cos(theta);
            vec.z =  params.radius * math::Sin(phi) * math::Sin(theta);

            streams.positions.emplace_back(vec.x);
            streams.positions.emplace_back(vec.y);
            streams.positions.emplace_back(vec.z);

            vec.Normalize();
            streams.normals.emplace_back(vec.x);
            streams.normals.emplace_back(vec.y);
            streams.normals.emplace_back(vec.z);

            streams.uvs.emplace_back(u);
            streams.uvs.emplace_back(1.0f - v);
        }
    }

    for (unsigned iy = 0; iy < params.height_segments; ++iy) {
        for (unsigned ix = 0; ix < params.width_segments; ++ix) {
            const auto a = ix + (params.width_segments + 1) * iy;
            const auto b = ix + (params.width_segments + 1) * (iy + 1);
            const auto c = ix + 1 + (params.width_segments + 1) * (iy + 1);
            const auto d = ix + 1 + (params.width_segments + 1) * iy;

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

SphereGeometry::SphereGeometry(const Parameters& params) {
    assert(params.radius > 0.0f);
    assert(params.width_segments >= 3);
    assert(params.height_segments >= 2);

    SetName("sphere geometry");

    auto streams = VertexStreams {};
    generate_geometry(params, streams);
    streams.AddTo(*this);
}

}
