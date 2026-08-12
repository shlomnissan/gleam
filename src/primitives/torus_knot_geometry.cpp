/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/primitives/torus_knot_geometry.hpp"

#include "vglx/math/utilities.hpp"
#include "vglx/math/vector3.hpp"

#include "geometries/vertex_streams.hpp"

#include <cassert>

namespace vglx {

namespace {

auto position_on_curve(float u, unsigned p, unsigned q, float radius) -> Vector3 {
    const auto qu_over_p = static_cast<float>(q) / static_cast<float>(p) * u;
    const auto cs = math::Cos(qu_over_p);
    const auto r = radius * (2.0f + cs) * 0.5f;

    return Vector3 {
        r * math::Cos(u),
        r * math::Sin(u),
        radius * math::Sin(qu_over_p) * 0.5f
    };
}

auto generate_geometry(
    const TorusKnotGeometry::Parameters& params,
    VertexStreams& streams
) {
    for (auto i = 0u; i <= params.tubular_segments; ++i) {
        const auto u = static_cast<float>(i) / static_cast<float>(params.tubular_segments)
                     * static_cast<float>(params.p) * math::two_pi;

        const auto p1 = position_on_curve(u, params.p, params.q, params.radius);
        const auto p2 = position_on_curve(u + 0.01f, params.p, params.q, params.radius);

        const auto t = p2 - p1;
        auto n = p2 + p1;
        auto b = Cross(t, n);
        n = Cross(b, t);
        b.Normalize();
        n.Normalize();

        for (auto j = 0u; j <= params.radial_segments; ++j) {
            const auto v = static_cast<float>(j) / static_cast<float>(params.radial_segments) * math::two_pi;
            const auto cx = -params.tube * math::Cos(v);
            const auto cy =  params.tube * math::Sin(v);

            auto vertex = Vector3 {
                p1.x + (cx * n.x + cy * b.x),
                p1.y + (cx * n.y + cy * b.y),
                p1.z + (cx * n.z + cy * b.z)
            };

            streams.positions.emplace_back(vertex.x);
            streams.positions.emplace_back(vertex.y);
            streams.positions.emplace_back(vertex.z);

            // the extrusion center is p1, so vertex - p1 points outward
            auto normal = (vertex - p1).Normalize();
            streams.normals.emplace_back(normal.x);
            streams.normals.emplace_back(normal.y);
            streams.normals.emplace_back(normal.z);

            streams.uvs.emplace_back(static_cast<float>(i) / static_cast<float>(params.tubular_segments));
            streams.uvs.emplace_back(static_cast<float>(j) / static_cast<float>(params.radial_segments));
        }
    }

    for (auto j = 1u; j <= params.tubular_segments; ++j) {
        for (auto i = 1u; i <= params.radial_segments; ++i) {
            const auto a = (params.radial_segments + 1) * (j - 1) + (i - 1);
            const auto b = (params.radial_segments + 1) * j + (i - 1);
            const auto c = (params.radial_segments + 1) * j + i;
            const auto d = (params.radial_segments + 1) * (j - 1) +  i;

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

TorusKnotGeometry::TorusKnotGeometry(const Parameters& params) {
    assert(params.radius > 0.0f);
    assert(params.tube > 0.0f);
    assert(params.tubular_segments >= 3);
    assert(params.radial_segments >= 3);
    assert(params.p >= 1);
    assert(params.q >= 1);

    SetName("torus knot geometry");

    auto streams = VertexStreams {};
    generate_geometry(params, streams);
    streams.AddTo(*this);
}

}
