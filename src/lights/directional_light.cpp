/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/lights/directional_light.hpp"

#include "vglx/materials/unlit_material.hpp"
#include "vglx/scene/mesh.hpp"

#include "utilities/assert.hpp"

namespace {

auto line_geometry() {
    auto geometry = vglx::Geometry::Create({0, 0, 0, 0, 0, 1});
    geometry->SetAttribute({vglx::Geometry::VertexAttributeType::Position, 3});
    geometry->primitive = vglx::Geometry::PrimitiveType::Lines;
    return geometry;
}

auto plane_geometry() {
    auto geometry = vglx::Geometry::Create({-1,  1, 0, 1,  1, 0, 1, -1, 0, -1, -1, 0});
    geometry->SetAttribute({vglx::Geometry::VertexAttributeType::Position, 3});
    geometry->primitive = vglx::Geometry::PrimitiveType::LineLoop;
    return geometry;
}

}

namespace vglx {

static constexpr auto debug_mesh_size = 0.5f;

struct DirectionalLight::Impl {
    Mesh* line {nullptr};
    Mesh* plane {nullptr};

    std::shared_ptr<UnlitMaterial> material;

    auto CreateDebugMesh(DirectionalLight* self) -> void {
        using enum Geometry::VertexAttributeType;
        using enum Geometry::PrimitiveType;

        material = UnlitMaterial::Create();
        material->two_sided = true;
        material->color = self->color;
        material->fog = false;

        line = self->Add(Mesh::Create(line_geometry(), material));
        plane = self->Add(Mesh::Create(plane_geometry(), material));
        plane->transform_auto_update = false;

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
        if (line != nullptr) {
            self->Remove(line);
            line = nullptr;
        }

        if (plane != nullptr) {
            self->Remove(plane);
            plane = nullptr;
        }

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
    if (target == nullptr) {
        return Normalize(GetWorldPosition());
    }

    VGLX_ASSERT(
        target->GetScene() == GetScene(),
        "SpotLight target must belong to the same scene"
    );

    return Normalize(GetWorldPosition() - target->GetWorldPosition());
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