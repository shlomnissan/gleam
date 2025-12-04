/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/lights/directional_light.hpp"

#include "vglx/materials/unlit_material.hpp"
#include "vglx/nodes/mesh.hpp"

namespace vglx {

static constexpr auto debug_mesh_size = 0.5f;

struct DirectionalLight::Impl {
    std::shared_ptr<Mesh> line;
    std::shared_ptr<Mesh> plane;
    std::shared_ptr<UnlitMaterial> material;

    auto CreateDebugMesh(DirectionalLight* self) -> void {
        using enum Geometry::VertexAttributeType;
        using enum Geometry::PrimitiveType;

        material = UnlitMaterial::Create();
        material->two_sided = true;
        material->color = self->color;
        material->fog = false;

        auto line_geometry = Geometry::Create({
            0, 0, 0,
            0, 0, 1
        });
        line_geometry->SetAttribute({Position, 3});
        line_geometry->primitive = Lines;

        line = Mesh::Create(line_geometry, material);
        line->transform_auto_update = false;
        self->Add(line);

        auto plane_geometry = Geometry::Create({
            -1,  1, 0,
             1,  1, 0,
             1, -1, 0,
            -1, -1, 0
        });
        plane_geometry->SetAttribute({Position, 3});
        plane_geometry->primitive = LineLoop;

        plane = Mesh::Create(plane_geometry, material);
        plane->transform_auto_update = false;
        self->Add(plane);

        UpdateDebugMesh(self);
    }

    auto UpdateDebugMesh(DirectionalLight* self) -> void {
        const auto target_world_pos = self->target != nullptr
            ? self->target->GetWorldPosition()
            : Vector3::Zero();

        plane->LookAt(target_world_pos);
        line->LookAt(target_world_pos);
        material->color = self->color;

        const auto length = (target_world_pos - self->GetWorldPosition()).Length();
        plane->SetScale(debug_mesh_size);
        line->SetScale({1.0f, 1.0f, length});
    }

    auto RemoveDebugMesh(DirectionalLight* self) -> void {
        if (line != nullptr) self->Remove(line);
        if (plane != nullptr) self->Remove(plane);
        line.reset();
        plane.reset();
        material.reset();
    }
};

DirectionalLight::DirectionalLight(const Parameters& params) :
    Light(params.color, params.intensity),
    target(params.target),
    impl_(std::make_unique<Impl>())
{
    SetName("directional light");
}

auto DirectionalLight::Direction() -> Vector3 {
    if (target != nullptr) {
        return Normalize(GetWorldPosition() - target->GetWorldPosition());
    }
    return Normalize(GetWorldPosition());
}

auto DirectionalLight::SetDebugMode(bool is_debug_mode) -> void {
    if (debug_mode_enabled_ != is_debug_mode) {
        is_debug_mode
        ? impl_->CreateDebugMesh(this)
        : impl_->RemoveDebugMesh(this);
        debug_mode_enabled_ = is_debug_mode;
    }
}

auto DirectionalLight::OnUpdate(float delta) -> void {
    if (debug_mode_enabled_) {
        impl_->UpdateDebugMesh(this);
    }
}

DirectionalLight::~DirectionalLight() = default;

}