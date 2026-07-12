/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/helpers/bounding_box.hpp"

#include "vglx/geometries/geometry.hpp"
#include "vglx/materials/unlit_material.hpp"
#include "vglx/scene/mesh.hpp"

#include <vector>

namespace vglx {

namespace {

auto create_geometry(const Box3& box) {
    auto vertices = std::vector<float> {
        box.max.x, box.max.y, box.max.z,
        box.min.x, box.max.y, box.max.z,
        box.min.x, box.min.y, box.max.z,
        box.max.x, box.min.y, box.max.z,
        box.max.x, box.max.y, box.min.z,
        box.min.x, box.max.y, box.min.z,
        box.min.x, box.min.y, box.min.z,
        box.max.x, box.min.y, box.min.z
    };

    auto indices = std::vector<unsigned> {
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7
    };

    auto geometry = Geometry::Create(vertices, indices);
    geometry->primitive = Geometry::PrimitiveType::Lines;
    geometry->SetAttribute({
        .type = Geometry::VertexAttributeType::Position,
        .item_size = 3
    });

    return geometry;
}

}

BoundingBox::BoundingBox(const Parameters& params) {
    Add(Mesh::Create(create_geometry(params.box), UnlitMaterial::Create({.color = params.color})));
}

}