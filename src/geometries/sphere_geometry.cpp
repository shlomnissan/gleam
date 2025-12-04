/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/geometries/sphere_geometry.hpp"

#include "vglx/math/vector3.hpp"
#include "vglx/math/utilities.hpp"

#include <cassert>

namespace vglx {

namespace {

auto generate_geometry(
    const SphereGeometry::Parameters& params,
    std::vector<float>& vertex_data,
    std::vector<unsigned int>& index_data
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

            // set position
            vertex_data.emplace_back(vec.x);
            vertex_data.emplace_back(vec.y);
            vertex_data.emplace_back(vec.z);

            // set normal
            vec.Normalize();
            vertex_data.emplace_back(vec.x);
            vertex_data.emplace_back(vec.y);
            vertex_data.emplace_back(vec.z);

            // set uv
            vertex_data.emplace_back(u);
            vertex_data.emplace_back(1.0f - v);
        }
    }

    for (unsigned iy = 0; iy < params.height_segments; ++iy) {
        for (unsigned ix = 0; ix < params.width_segments; ++ix) {
            const auto a = ix + (params.width_segments + 1) * iy;
            const auto b = ix + (params.width_segments + 1) * (iy + 1);
            const auto c = ix + 1 + (params.width_segments + 1) * (iy + 1);
            const auto d = ix + 1 + (params.width_segments + 1) * iy;

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

SphereGeometry::SphereGeometry(const Parameters& params) {
    assert(params.radius > 0.0f);
    assert(params.width_segments >= 3);
    assert(params.height_segments >= 2);

    SetName("sphere geometry");

    generate_geometry(params, vertex_data_, index_data_);

    SetAttribute({.type = Geometry::VertexAttributeType::Position, .item_size = 3});
    SetAttribute({.type = Geometry::VertexAttributeType::Normal, .item_size = 3});
    SetAttribute({.type = Geometry::VertexAttributeType::UV, .item_size = 2});
}

}