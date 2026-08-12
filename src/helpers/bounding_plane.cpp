/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/helpers/bounding_plane.hpp"

#include "vglx/geometries/buffer_attribute.hpp"
#include "vglx/geometries/geometry.hpp"
#include "vglx/geometries/wireframe_geometry.hpp"
#include "vglx/materials/unlit_material.hpp"
#include "vglx/math/color.hpp"
#include "vglx/scene/mesh.hpp"

#include <utility>
#include <vector>

namespace vglx {

namespace {

auto create_positions(std::vector<float> data) {
    return BufferAttribute::Create({
        .name = BufferAttribute::kPosition,
        .format = BufferAttribute::Format::Float32x3,
        .rate = BufferAttribute::Rate::Vertex
    }, std::move(data));
}

auto create_wireframe_mesh(const Color& color) {
    auto geometry = Geometry::Create();
    geometry->AddAttribute(create_positions({
         1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
         0.0f,  0.0f, 0.0f
    }));
    geometry->SetIndices({ 0, 1, 2, 3, 4, 5, 6, 7, 8 });

    auto wireframe_geometry = WireframeGeometry::Create(geometry.get());
    auto wireframe_material = UnlitMaterial::Create({.color = color});
    wireframe_material->side = Material::Side::TwoSided;

    return Mesh::Create(wireframe_geometry, wireframe_material);
}

auto create_solid_mesh(const Color& color) {
    auto geometry = Geometry::Create();
    geometry->AddAttribute(create_positions({
         1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f
    }));

    auto solid_material = UnlitMaterial::Create({.color = color});
    solid_material->opacity = 0.2f;
    solid_material->transparent = true;

    return Mesh::Create(geometry, solid_material);
}

}

BoundingPlane::BoundingPlane(const Parameters& params) {
    Add(create_wireframe_mesh(params.color));
    Add(create_solid_mesh(params.color));
    transform.SetScale({params.size * 0.5f, params.size * 0.5f, params.size});
    LookAt(params.plane.normal);
    transform.Translate({0.0f, 0.0f, -params.plane.distance});
}

}