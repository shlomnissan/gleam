/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/primitives/cylinder_geometry.hpp"

#include "vglx/math/utilities.hpp"
#include "vglx/math/vector3.hpp"

#include <cassert>

namespace vglx {

namespace {

auto generate_torso(
    const CylinderGeometry::Parameters& params,
    std::vector<float>& vertex_data,
    std::vector<unsigned int>& index_data
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

            vertex_data.emplace_back(radius * sin_theta); // pos x
            vertex_data.emplace_back(-v * params.height + half_height); // pos y
            vertex_data.emplace_back(radius * cos_theta); // pos z
            vertex_data.emplace_back(normal.x); // normal x
            vertex_data.emplace_back(normal.y); // normal y
            vertex_data.emplace_back(normal.z); // normal z
            vertex_data.emplace_back(u); // u
            vertex_data.emplace_back(1.0f - v); // v
        }
    }

    for (auto y = 0; y < params.height_segments; ++y) {
        for (auto x = 0; x < params.radial_segments; ++x) {
            const auto a = x + (params.radial_segments + 1) * y;
            const auto b = x + (params.radial_segments + 1) * (y + 1);
            const auto c = x + 1 + (params.radial_segments + 1) * (y + 1);
            const auto d = x + 1 + (params.radial_segments + 1) * y;

            index_data.emplace_back(a);
            index_data.emplace_back(b);
            index_data.emplace_back(d);
            index_data.emplace_back(b);
            index_data.emplace_back(c);
            index_data.emplace_back(d);
        }
    }
}

auto generate_cap(
    const CylinderGeometry::Parameters& params,
    bool top,
    std::vector<float>& vertex_data,
    std::vector<unsigned int>& index_data
) {
    const auto half_height = params.height / 2;
    const auto sign = top ? 1.0f : -1.0f;
    const auto center_index_start = vertex_data.size() / 8;

    // Generate the center of the cap (multiple vertices at the same position)
    for (auto x = 0; x < params.radial_segments; ++x) {
        vertex_data.emplace_back(0.0f); // pos x
        vertex_data.emplace_back(half_height * sign); // pos y
        vertex_data.emplace_back(0.0f); // pos z
        vertex_data.emplace_back(0.0f); // normal x
        vertex_data.emplace_back(sign); // normal y
        vertex_data.emplace_back(0.0f); // normal z
        vertex_data.emplace_back(0.5f); // u
        vertex_data.emplace_back(0.5f); // v
    }

    const auto radius = top ? params.radius_top : params.radius_bottom;
    const auto center_index_end = vertex_data.size() / 8;

    // Generate the vertices around the center
    for (auto x = 0; x <= params.radial_segments; ++x) {
        const auto u = static_cast<float>(x) / static_cast<float>(params.radial_segments);
        const auto theta = u * math::two_pi;
        const auto cos_theta = math::Cos(theta);
        const auto sin_theta = math::Sin(theta);

        vertex_data.emplace_back(radius * sin_theta); // pos x
        vertex_data.emplace_back(half_height * sign); // pos y
        vertex_data.emplace_back(radius * cos_theta); // pos z
        vertex_data.emplace_back(0.0f); // normal x
        vertex_data.emplace_back(sign); // normal y
        vertex_data.emplace_back(0.0f); // normal z
        vertex_data.emplace_back(cos_theta * 0.5f + 0.5f); // u
        vertex_data.emplace_back(sin_theta * 0.5f * sign + 0.5f);  // v
    }

    for (auto x = 0; x < params.radial_segments; ++x) {
        const auto c = static_cast<unsigned int>(center_index_start + x);
        const auto i = static_cast<unsigned int>(center_index_end + x);

        if (top) {
            index_data.emplace_back(i);
            index_data.emplace_back(i + 1);
            index_data.emplace_back(c);
        } else {
            index_data.emplace_back(i + 1);
            index_data.emplace_back(i);
            index_data.emplace_back(c);
        }
    }
}

}

CylinderGeometry::CylinderGeometry(const Parameters& params) {
    assert(params.height > 0.0f);
    assert(params.radial_segments > 0);
    assert(params.height_segments > 0);

    SetName("cylinder geometry");

    generate_torso(params, vertex_data_, index_data_);
    if (!params.open_ended) {
        if (params.radius_top > 0.0f) generate_cap(params, true, vertex_data_, index_data_);
        if (params.radius_bottom > 0.0f) generate_cap(params, false, vertex_data_, index_data_);
    }

    SetAttribute({.type = Geometry::VertexAttributeType::Position, .item_size = 3});
    SetAttribute({.type = Geometry::VertexAttributeType::Normal, .item_size = 3});
    SetAttribute({.type = Geometry::VertexAttributeType::UV, .item_size = 2});
}

}