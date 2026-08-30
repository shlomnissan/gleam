/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/helpers/arrow.hpp"

#include "vglx/geometries/buffer_attribute.hpp"
#include "vglx/geometries/geometry.hpp"
#include "vglx/materials/unlit_material.hpp"
#include "vglx/math/utilities.hpp"
#include "vglx/math/vector3.hpp"
#include "vglx/primitives/cone_geometry.hpp"
#include "vglx/scene/mesh.hpp"

namespace vglx {

namespace {

constexpr auto kConeHeight = 0.1f;

auto line_geometry(float length) {
    const auto line_geometry = Geometry::Create();
    line_geometry->AddAttribute(BufferAttribute::Create({
        .name = BufferAttribute::kPosition,
        .format = BufferAttribute::Format::Float32x3,
        .rate = BufferAttribute::Rate::Vertex
    }, {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, length
    }));
    line_geometry->primitive = Geometry::PrimitiveType::Lines;
    return line_geometry;
}

}

Arrow::Arrow(const Parameters& params) {
    const auto material = UnlitMaterial::Create({.color = params.color});
    const auto cone = Add(Mesh::Create(
        ConeGeometry::Create({.radius = 0.03f, .height = kConeHeight}),
        material
    ));

    Add(Mesh::Create(line_geometry(params.length - kConeHeight), material));

    cone->transform.Translate({0.0f, 0.0f, params.length - kConeHeight / 2.0f});
    cone->transform.Rotate(Vector3::UnitX(), math::pi_over_2);

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
