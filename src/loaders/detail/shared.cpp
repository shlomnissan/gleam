/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "loaders/detail/shared.hpp"

#include "vglx/math/utilities.hpp"
#include "vglx/math/vector3.hpp"

#include <cmath>

namespace vglx::detail {

namespace {

auto read_vec3(const std::vector<float>& data, uint32_t idx) -> Vector3 {
    return {data[idx * 3 + 0], data[idx * 3 + 1], data[idx * 3 + 2]};
}

auto read_vec2(const std::vector<float>& data, uint32_t idx) -> Vector2 {
    return {data[idx * 2 + 0], data[idx * 2 + 1]};
}

}

auto generate_normals(
    const std::vector<float>& positions,
    const std::vector<uint32_t>& indices
) -> std::vector<float> {
    const auto vertex_count = positions.size() / 3;

    auto normals = std::vector<float>(positions.size(), 0.0f);

    const auto index_count = indices.empty() ? vertex_count : indices.size();
    for (size_t i = 0; i + 2 < index_count; i += 3) {
        const auto i0 = indices.empty() ? static_cast<uint32_t>(i + 0) : indices[i + 0];
        const auto i1 = indices.empty() ? static_cast<uint32_t>(i + 1) : indices[i + 1];
        const auto i2 = indices.empty() ? static_cast<uint32_t>(i + 2) : indices[i + 2];

        const auto v0 = read_vec3(positions, i0);
        const auto v1 = read_vec3(positions, i1);
        const auto v2 = read_vec3(positions, i2);

        const auto f = Cross(v1 - v0, v2 - v0);
        if (Dot(f, f) <= math::eps * math::eps) continue;

        for (auto idx : {i0, i1, i2}) {
            normals[idx * 3 + 0] += f.x;
            normals[idx * 3 + 1] += f.y;
            normals[idx * 3 + 2] += f.z;
        }
    }

    for (size_t i = 0; i < vertex_count; ++i) {
        auto n = read_vec3(normals, i);
        if (n.Length() > 0.0f) {
            n.Normalize();
            normals[i * 3 + 0] = n.x;
            normals[i * 3 + 1] = n.y;
            normals[i * 3 + 2] = n.z;
        }
    }

    return normals;
}

auto generate_tangents(
    const std::vector<float>& positions,
    const std::vector<float>& normals,
    const std::vector<float>& uvs,
    const std::vector<uint32_t>& indices
) -> std::vector<float> {
    const auto vertex_count = positions.size() / 3;

    auto tangents = std::vector<float>(vertex_count * 4, 0.0f);
    auto t_accum = std::vector<Vector3>(vertex_count);
    auto b_accum = std::vector<Vector3>(vertex_count);

    const auto index_count = indices.empty() ? vertex_count : indices.size();
    for (size_t i = 0; i + 2 < index_count; i += 3) {
        const auto i0 = indices.empty() ? static_cast<uint32_t>(i + 0) : indices[i + 0];
        const auto i1 = indices.empty() ? static_cast<uint32_t>(i + 1) : indices[i + 1];
        const auto i2 = indices.empty() ? static_cast<uint32_t>(i + 2) : indices[i + 2];

        const auto v0 = read_vec3(positions, i0);
        const auto v1 = read_vec3(positions, i1);
        const auto v2 = read_vec3(positions, i2);

        const auto w0 = read_vec2(uvs, i0);
        const auto w1 = read_vec2(uvs, i1);
        const auto w2 = read_vec2(uvs, i2);

        const auto e0 = v1 - v0;
        const auto e1 = v2 - v0;
        const auto uv0 = w1 - w0;
        const auto uv1 = w2 - w0;

        const auto f = Cross(e0, e1);
        if (Dot(f, f) <= math::eps * math::eps) continue;

        const auto det = (uv0.x * uv1.y - uv1.x * uv0.y);
        if (std::fabs(det) < math::eps) continue;
        const auto r = 1.0f / det;

        const auto tangent = Vector3 {
            (e0.x * uv1.y - e1.x * uv0.y) * r,
            (e0.y * uv1.y - e1.y * uv0.y) * r,
            (e0.z * uv1.y - e1.z * uv0.y) * r
        };

        const auto bitangent = Vector3 {
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
        const auto n = read_vec3(normals, i);
        const auto b = b_accum[i];

        auto t = t_accum[i] - Dot(n, t_accum[i]) * n;
        if (Dot(t, t) <= math::eps * math::eps) {
            const auto axis = std::fabs(n.x) < 0.9f
                ? Vector3 {1.0f, 0.0f, 0.0f}
                : Vector3 {0.0f, 1.0f, 0.0f};
            t = Cross(n, axis);
        }
        t = Normalize(t);
        const auto s = Dot(Cross(n, t), b) >= 0 ? 1.0f : -1.0f;

        tangents[i * 4 + 0] = t.x;
        tangents[i * 4 + 1] = t.y;
        tangents[i * 4 + 2] = t.z;
        tangents[i * 4 + 3] = s;
    }

    return tangents;
}

}
