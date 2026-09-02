/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/helpers/bounding_box.hpp"

#include "vglx/geometries/buffer_attribute.hpp"
#include "vglx/geometries/geometry.hpp"
#include "vglx/materials/unlit_material.hpp"
#include "vglx/math/box3.hpp"

#include <vector>

namespace vglx {

namespace {

auto create_geometry(const Box3& box) {
    auto geometry = Geometry::Create();
    geometry->AddAttribute(BufferAttribute::Create({
        .name = BufferAttribute::kPosition,
        .format = BufferAttribute::Format::Float32x3,
        .rate = BufferAttribute::Rate::Vertex
    }, {
        box.max.x, box.max.y, box.max.z,
        box.min.x, box.max.y, box.max.z,
        box.min.x, box.min.y, box.max.z,
        box.max.x, box.min.y, box.max.z,
        box.max.x, box.max.y, box.min.z,
        box.min.x, box.max.y, box.min.z,
        box.min.x, box.min.y, box.min.z,
        box.max.x, box.min.y, box.min.z
    }));

    geometry->SetIndices({
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7
    });

    geometry->primitive = Geometry::PrimitiveType::Lines;

    return geometry;
}

}

BoundingBox::BoundingBox(const Parameters& params) {
    SetGeometry(create_geometry(params.box));
    SetMaterial(UnlitMaterial::Create({.color = params.color}));
}

}