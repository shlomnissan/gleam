/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/helpers/bounding_sphere.hpp"

#include "vglx/geometries/buffer_attribute.hpp"
#include "vglx/geometries/geometry.hpp"
#include "vglx/materials/unlit_material.hpp"
#include "vglx/math/sphere.hpp"
#include "vglx/math/utilities.hpp"

#include <cstdint>
#include <utility>
#include <vector>

namespace vglx {

namespace {

auto create_geometry(const Sphere& sphere) {
    constexpr auto segments = uint32_t {64};

    std::vector<float> vertices;
    std::vector<unsigned> indices;

    const auto center = sphere.center;
    const auto radius = sphere.radius;

    enum class Axis { XY, XZ, YZ };

    for (auto axis = 0; axis < 3; ++axis) {
        auto offset = static_cast<unsigned int>(vertices.size() / 3);
        for (auto i = 0; i <= segments; ++i) {
            auto p = center;
            auto theta = (static_cast<float>(i) / static_cast<float>(segments)) * math::two_pi;

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

    auto geometry = Geometry::Create();
    geometry->AddAttribute(BufferAttribute::Create({
        .name = BufferAttribute::kPosition,
        .format = BufferAttribute::Format::Float32x3,
        .rate = BufferAttribute::Rate::Vertex
    }, std::move(vertices)));
    geometry->SetIndices(std::move(indices));
    geometry->primitive = Geometry::PrimitiveType::Lines;

    return geometry;
}

}

BoundingSphere::BoundingSphere(const Parameters& params) {
    SetGeometry(create_geometry(params.sphere));
    SetMaterial(UnlitMaterial::Create({.color = params.color}));
}

}
