/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "shared.hpp"

#include <vglx/math/vector3.hpp>

#include <cassert>
#include <cmath>

namespace vglx::detail {

auto make_layout(bool has_uvs, bool has_colors) -> VertexLayout {
    auto layout = VertexLayout {};
    auto offset = uint32_t {0};

    layout.has_uvs = has_uvs;
    layout.has_tangents = has_uvs;
    layout.has_colors = has_colors;

    layout.position_offset = offset;
    offset += 3;

    layout.normal_offset = offset;
    offset += 3;

    if (layout.has_uvs) {
        layout.uv_offset = offset;
        offset += 2;
    }

    if (layout.has_tangents) {
        layout.tangent_offset = offset;
        offset += 4;
    }

    if (layout.has_colors) {
        layout.color_offset = offset;
        offset += 3;
    }

    layout.stride = offset;

    return layout;
}

auto generate_normals(
    std::vector<float>& vertex_data,
    std::vector<unsigned>& index_data,
    const VertexLayout& layout
) -> void {
    for (size_t i = 0; i < index_data.size(); i += 3) {
        auto i0 = index_data[i + 0];
        auto i1 = index_data[i + 1];
        auto i2 = index_data[i + 2];

        auto v0 = Vector3 {
            vertex_data[i0 * layout.stride + layout.position_offset + 0],
            vertex_data[i0 * layout.stride + layout.position_offset + 1],
            vertex_data[i0 * layout.stride + layout.position_offset + 2]
        };

        auto v1 = Vector3 {
            vertex_data[i1 * layout.stride + layout.position_offset + 0],
            vertex_data[i1 * layout.stride + layout.position_offset + 1],
            vertex_data[i1 * layout.stride + layout.position_offset + 2]
        };

        auto v2 = Vector3 {
            vertex_data[i2 * layout.stride + layout.position_offset + 0],
            vertex_data[i2 * layout.stride + layout.position_offset + 1],
            vertex_data[i2 * layout.stride + layout.position_offset + 2]
        };

        auto e0 = v1 - v0;
        auto e1 = v2 - v0;
        auto f = Cross(e0, e1);
        if (Dot(f, f) <= math::eps * math::eps) continue;

        for (auto idx : {i0, i1, i2}) {
            vertex_data[idx * layout.stride + layout.normal_offset + 0] += f.x;
            vertex_data[idx * layout.stride + layout.normal_offset + 1] += f.y;
            vertex_data[idx * layout.stride + layout.normal_offset + 2] += f.z;
        }
    }

    auto vertex_count = vertex_data.size() / layout.stride;
    for (size_t i = 0; i < vertex_count; ++i) {
        auto n = Vector3 {
            vertex_data[i * layout.stride + layout.normal_offset + 0],
            vertex_data[i * layout.stride + layout.normal_offset + 1],
            vertex_data[i * layout.stride + layout.normal_offset + 2]
        };

        if (n.Length() > 0.0f) {
            n.Normalize();
            vertex_data[i * layout.stride + layout.normal_offset + 0] = n.x;
            vertex_data[i * layout.stride + layout.normal_offset + 1] = n.y;
            vertex_data[i * layout.stride + layout.normal_offset + 2] = n.z;
        }
    }
}

auto generate_tangents(
    std::vector<float>& vertex_data,
    std::vector<unsigned>& index_data,
    const VertexLayout& layout
) -> void {
    assert(layout.has_uvs && layout.has_tangents);
    assert(layout.uv_offset && layout.tangent_offset);

    auto uv_offset = layout.uv_offset.value();
    auto tan_offset = layout.tangent_offset.value();

    auto vertex_count = vertex_data.size() / layout.stride;
    auto t_accum = std::vector<Vector3>(vertex_count);
    auto b_accum = std::vector<Vector3>(vertex_count);

    for (size_t i = 0; i < index_data.size(); i += 3) {
        auto i0 = index_data[i + 0];
        auto i1 = index_data[i + 1];
        auto i2 = index_data[i + 2];

        auto v0 = Vector3 {
            vertex_data[i0 * layout.stride + layout.position_offset + 0],
            vertex_data[i0 * layout.stride + layout.position_offset + 1],
            vertex_data[i0 * layout.stride + layout.position_offset + 2]
        };

        auto v1 = Vector3 {
            vertex_data[i1 * layout.stride + layout.position_offset + 0],
            vertex_data[i1 * layout.stride + layout.position_offset + 1],
            vertex_data[i1 * layout.stride + layout.position_offset + 2]
        };

        auto v2 = Vector3 {
            vertex_data[i2 * layout.stride + layout.position_offset + 0],
            vertex_data[i2 * layout.stride + layout.position_offset + 1],
            vertex_data[i2 * layout.stride + layout.position_offset + 2]
        };

        auto w0 = Vector2 {
            vertex_data[i0 * layout.stride + uv_offset + 0],
            vertex_data[i0 * layout.stride + uv_offset + 1]
        };

        auto w1 = Vector2 {
            vertex_data[i1 * layout.stride + uv_offset + 0],
            vertex_data[i1 * layout.stride + uv_offset + 1]
        };

        auto w2 = Vector2 {
            vertex_data[i2 * layout.stride + uv_offset + 0],
            vertex_data[i2 * layout.stride + uv_offset + 1]
        };

        auto e0 = v1 - v0;
        auto e1 = v2 - v0;
        auto uv0 = w1 - w0;
        auto uv1 = w2 - w0;

        auto f = Cross(e0, e1);
        if (Dot(f, f) <= math::eps * math::eps) continue;

        auto det = (uv0.x * uv1.y - uv1.x * uv0.y);
        if (std::fabs(det) < math::eps) continue;
        auto r = 1.0f / det;

        auto tangent = Vector3 {
            (e0.x * uv1.y - e1.x * uv0.y) * r,
            (e0.y * uv1.y - e1.y * uv0.y) * r,
            (e0.z * uv1.y - e1.z * uv0.y) * r
        };

        auto bitangent = Vector3 {
            (e1.x * uv0.x - e0.x * uv1.x) * r,
            (e1.y * uv0.x - e0.y * uv1.x) * r,
            (e1.z * uv0.x - e0.z * uv1.x) * r
        };

        t_accum[i0] += tangent;
        t_accum[i1] += tangent;
        t_accum[i2] += tangent;

        b_accum[i0] += bitangent;
        b_accum[i1] += bitangent;
        b_accum[i2] += bitangent;
    }

    for (size_t i = 0; i < vertex_count; ++i) {
        auto n = Vector3 {
            vertex_data[i * layout.stride + layout.normal_offset + 0],
            vertex_data[i * layout.stride + layout.normal_offset + 1],
            vertex_data[i * layout.stride + layout.normal_offset + 2]
        };

        auto t = t_accum[i];
        auto b = b_accum[i];

        t = Normalize(t - Dot(n, t) * n);
        auto s = Dot(Cross(n, t), b) >= 0 ? 1.0f : -1.0f;

        vertex_data[i * layout.stride + tan_offset + 0] = t.x;
        vertex_data[i * layout.stride + tan_offset + 1] = t.y;
        vertex_data[i * layout.stride + tan_offset + 2] = t.z;
        vertex_data[i * layout.stride + tan_offset + 3] = s;
    }
}

}