/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/lights/spot_light.hpp"

#include "vglx/materials/unlit_material.hpp"
#include "vglx/math/utilities.hpp"
#include "vglx/scene/mesh.hpp"

#include "utilities/assert.hpp"

#include <vector>

namespace {

auto lines_geometry() {
    // lines for the cone
    auto points = std::vector<float> {
        0, 0, 0,  0,  0, 1,
        0, 0, 0,  1,  0, 1,
        0, 0, 0, -1,  0, 1,
        0, 0, 0,  0,  1, 1,
        0, 0, 0,  0, -1, 1,
    };

    // circle for the cone base
    static constexpr auto circle_line_segments = 64;
    for (unsigned i = 0, j = 1, l = circle_line_segments; i < l; i++, j++) {
        const auto p1 = (static_cast<float>(i) / static_cast<float>(l)) * vglx::math::two_pi;
        const auto p2 = (static_cast<float>(j) / static_cast<float>(l)) * vglx::math::two_pi;
        points.insert(points.end(), {
            vglx::math::Cos(p1), vglx::math::Sin(p1), 1.0f,
            vglx::math::Cos(p2), vglx::math::Sin(p2), 1.0f
        });
    }

    auto geometry = vglx::Geometry::Create(points);
    geometry->SetName("directional light line");
    geometry->SetAttribute({vglx::Geometry::VertexAttributeType::Position, 3});
    geometry->primitive = vglx::Geometry::PrimitiveType::Lines;
    return geometry;
}

}

namespace vglx {

struct SpotLight::Impl {
    Mesh* cone {nullptr};

    std::shared_ptr<UnlitMaterial> material;

    auto CreateDebugMesh(SpotLight* self) -> void {
        using enum Geometry::VertexAttributeType;
        using enum Geometry::PrimitiveType;

        material = UnlitMaterial::Create();
        material->two_sided = true;
        material->color = self->color;
        material->fog = false;

        cone = self->Add(Mesh::Create(lines_geometry(), material));

        UpdateDebugMesh(self);
    }

    auto UpdateDebugMesh(SpotLight* self) -> void {
        const auto target_world_pos = self->target != nullptr
            ? self->target->GetWorldPosition()
            : Vector3::Zero();

        const auto cone_length = (target_world_pos - self->GetWorldPosition()).Length() + 1.0f;
        const auto cone_width = std::tan(self->angle) * cone_length;

        cone->LookAt(target_world_pos);
        cone->transform.SetScale({cone_width, cone_width, cone_length});
        material->color = self->color;
    }

    auto RemoveDebugMesh(SpotLight* self) -> void {
        if (cone != nullptr) {
            self->Remove(cone);
            cone = nullptr;
        }

        material.reset();
    }
};

SpotLight::SpotLight(const Parameters& params) :
    Light(params.color, params.intensity),
    angle(params.angle),
    penumbra(params.penumbra),
    target(params.target),
    attenuation(params.attenuation),
    cast_shadow(params.cast_shadow),
    impl_(std::make_unique<Impl>())
{
    SetName("spot light");
}

auto SpotLight::Direction() -> Vector3 {
    if (target == nullptr) {
        return Normalize(GetWorldPosition());
    }

    VGLX_ASSERT(
        target->GetScene() == GetScene(),
        "SpotLight target must belong to the same scene"
    );

    return Normalize(GetWorldPosition() - target->GetWorldPosition());
}

auto SpotLight::SetDebugMode(bool is_debug_mode) -> void {
    if (debug_mode_enabled_ != is_debug_mode) {
        is_debug_mode
        ? impl_->CreateDebugMesh(this)
        : impl_->RemoveDebugMesh(this);
        debug_mode_enabled_ = is_debug_mode;
    }
}

auto SpotLight::OnUpdate(float delta) -> void {
    if (debug_mode_enabled_) {
        impl_->UpdateDebugMesh(this);
    }
}

[[nodiscard]] auto SpotLight::GetShadow() const -> const Shadow* {
    return cast_shadow ? &shadow : nullptr;
}

SpotLight::~SpotLight() = default;

}