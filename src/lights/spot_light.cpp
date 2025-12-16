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

#include <vector>

namespace vglx {

struct SpotLight::Impl {
    std::shared_ptr<Mesh> cone;
    std::shared_ptr<UnlitMaterial> material;

    auto CreateDebugMesh(SpotLight* self) -> void {
        using enum Geometry::VertexAttributeType;
        using enum Geometry::PrimitiveType;

        material = UnlitMaterial::Create();
        material->two_sided = true;
        material->color = self->color;
        material->fog = false;

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
            const auto p1 = (static_cast<float>(i) / static_cast<float>(l)) * math::two_pi;
            const auto p2 = (static_cast<float>(j) / static_cast<float>(l)) * math::two_pi;
            points.insert(points.end(), {
                math::Cos(p1), math::Sin(p1), 1.0f,
                math::Cos(p2), math::Sin(p2), 1.0f
            });
        }

        auto cone_geometry = Geometry::Create(points);
        cone_geometry->SetName("directional light line");
        cone_geometry->SetAttribute({Position, 3});
        cone_geometry->primitive = Lines;

        cone = Mesh::Create(cone_geometry, material);
        cone->transform_auto_update = false;

        self->Add(cone);
        UpdateDebugMesh(self);
    }

    auto UpdateDebugMesh(SpotLight* self) -> void {
        const auto target_world_pos = self->target != nullptr
            ? self->target->GetWorldPosition()
            : Vector3::Zero();

        const auto cone_length = (target_world_pos - self->GetWorldPosition()).Length() + 1.0f;
        const auto cone_width = std::tan(self->angle) * cone_length;

        cone->LookAt(target_world_pos);
        cone->SetScale({cone_width, cone_width, cone_length});
        material->color = self->color;
    }

    auto RemoveDebugMesh(SpotLight* self) -> void {
        if (cone != nullptr) self->Remove(cone);
        cone.reset();
        material.reset();
    }
};

SpotLight::SpotLight(const Parameters& params) :
    Light(params.color, params.intensity),
    angle(params.angle),
    penumbra(params.penumbra),
    target(params.target),
    attenuation(params.attenuation),
    impl_(std::make_unique<Impl>())
{
    SetName("spot light");
}

auto SpotLight::Direction() -> Vector3 {
    if (target != nullptr) {
        return Normalize(GetWorldPosition() - target->GetWorldPosition());
    }
    return Normalize(GetWorldPosition());
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

SpotLight::~SpotLight() = default;

}