/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/helpers/grid.hpp"

#include "vglx/geometries/buffer_attribute.hpp"
#include "vglx/geometries/geometry.hpp"
#include "vglx/materials/unlit_material.hpp"

#include <utility>
#include <vector>

namespace vglx {

Grid::Grid(const Parameters& params) {
    const auto step = params.size / params.divisions;
    const auto half_size = params.size / 2;
    auto vertices = std::vector<float> {};
    auto k = -half_size;
    for (auto i = 0, j = 0; i <= params.divisions; i++) {
        vertices.insert(vertices.end(), {
           -half_size, 0.0f, k,
            half_size, 0.0f, k,
            k, 0.0f, -half_size,
            k, 0.0f, half_size
        });
        k += step;
    }

    auto geometry = Geometry::Create();
    geometry->AddAttribute(BufferAttribute::Create({
        .name = BufferAttribute::kPosition,
        .format = BufferAttribute::Format::Float32x3,
        .rate = BufferAttribute::Rate::Vertex
    }, std::move(vertices)));
    geometry->primitive = Geometry::PrimitiveType::Lines;
    geometry->SetName("grid");

    SetGeometry(geometry);
    SetMaterial(UnlitMaterial::Create({.color = params.color}));
}

}
