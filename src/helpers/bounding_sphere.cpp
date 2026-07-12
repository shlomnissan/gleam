/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/helpers/bounding_sphere.hpp"

#include "vglx/geometries/geometry.hpp"
#include "vglx/materials/unlit_material.hpp"
#include "vglx/math/utilities.hpp"
#include "vglx/scene/mesh.hpp"

namespace vglx {

namespace {

auto create_geometry(const Sphere& sphere) {
    constexpr auto segments = 64.0f;

    std::vector<float> vertices;
    std::vector<unsigned> indices;

    const auto center = sphere.center;
    const auto radius = sphere.radius;

    enum class Axis { XY, XZ, YZ };

    for (auto axis = 0; axis < 3; ++axis) {
        auto offset = static_cast<unsigned int>(vertices.size() / 3);
        for (auto i = 0; i <= static_cast<unsigned>(segments); ++i) {
            auto p = center;
            auto theta = (static_cast<float>(i) / segments) * math::two_pi;

            if (axis == static_cast<unsigned>(Axis::XY)) {
                p.x += radius * math::Cos(theta);
                p.y += radius * math::Sin(theta);
            }
            if (axis == static_cast<unsigned>(Axis::XZ)) {
                p.x += radius * math::Cos(theta);
                p.z += radius * math::Sin(theta);
            }
            if (axis == static_cast<unsigned>(Axis::YZ)) {
                p.y += radius * math::Cos(theta);
                p.z += radius * math::Sin(theta);
            }

            vertices.emplace_back(p.x);
            vertices.emplace_back(p.y);
            vertices.emplace_back(p.z);

            if (i > 0) {
                indices.emplace_back(offset + i - 1);
                indices.emplace_back(offset + i);
            }
        }
    }

    auto geometry = Geometry::Create(vertices, indices);
    geometry->primitive = Geometry::PrimitiveType::Lines;
    geometry->SetAttribute({
        .type = Geometry::VertexAttributeType::Position,
        .item_size = 3
    });

    return geometry;
}

}

BoundingSphere::BoundingSphere(const Parameters& params) {
    Add(Mesh::Create(create_geometry(params.sphere), UnlitMaterial::Create({.color = params.color})));
}

}