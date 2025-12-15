/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/helpers/arrow.hpp"

#include "vglx/geometries/cone_geometry.hpp"
#include "vglx/geometries/geometry.hpp"
#include "vglx/materials/unlit_material.hpp"
#include "vglx/nodes/mesh.hpp"

namespace vglx {

Arrow::Arrow(const Parameters& params) {
    const auto material = UnlitMaterial::Create(params.color);
    const auto cone_height = 0.1f;
    const auto cone = Mesh::Create(ConeGeometry::Create({
        .radius = 0.03f,
        .height = cone_height
    }), material);
    cone->TranslateZ(params.length - cone_height / 2.0f);
    cone->RotateX(math::pi_over_2);
    Add(cone);

    const auto geometry = Geometry::Create({
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, params.length - cone_height
    });
    geometry->SetAttribute({Geometry::VertexAttributeType::Position, 3});
    geometry->primitive = Geometry::PrimitiveType::Lines;
    Add(Mesh::Create(geometry, material));

    SetOrigin(params.origin);
    SetDirection(params.direction);
}

auto Arrow::SetDirection(const Vector3& direction) -> void {
    if (direction.LengthSquared() == 0.0f) return;
    LookAt(GetWorldPosition() + Normalize(direction));
}

auto Arrow::SetOrigin(const Vector3& origin) -> void {
    transform.SetPosition(origin);
}

}