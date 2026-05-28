/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/helpers/bounding_plane.hpp"

#include "vglx/geometries/geometry.hpp"
#include "vglx/geometries/wireframe_geometry.hpp"
#include "vglx/materials/unlit_material.hpp"
#include "vglx/scene/mesh.hpp"

#include <vector>

namespace vglx {

namespace {

auto create_wireframe_mesh(const Color& color) {
    auto geometry = Geometry::Create({
         1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
         0.0f,  0.0f, 0.0f
    }, { 0, 1, 2, 3, 4, 5, 6, 7, 8 });
    geometry->SetAttribute({Geometry::VertexAttributeType::Position, 3});

    auto wireframe_geometry = WireframeGeometry::Create(geometry.get());
    auto wireframe_material = UnlitMaterial::Create({.color = color});
    wireframe_material->two_sided = true;

    return Mesh::Create(wireframe_geometry, wireframe_material);
}

auto create_solid_mesh(const Color& color) {
    auto geometry = Geometry::Create({
         1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f
    });

    geometry->SetAttribute({Geometry::VertexAttributeType::Position, 3});

    auto solid_material = UnlitMaterial::Create({.color = color});
    solid_material->opacity = 0.2f;
    solid_material->transparent = true;

    return Mesh::Create(geometry, solid_material);
}

}

BoundingPlane::BoundingPlane(const Plane& plane, float size, const Color& color) {
    Add(create_wireframe_mesh(color));
    Add(create_solid_mesh(color));
    transform.SetScale({size * 0.5f, size * 0.5f, size});
    LookAt(plane.normal);
    transform.Translate({0.0f, 0.0f, -plane.distance});
}

}