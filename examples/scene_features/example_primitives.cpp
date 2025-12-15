/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "example_primitives.hpp"

#include "ui_helpers.hpp"

#include <vglx/helpers.hpp>
#include <vglx/lights.hpp>
#include <vglx/materials.hpp>

using namespace vglx;
using namespace vglx::math;

ExamplePrimitives::ExamplePrimitives() {
    InitializeParams();

    Add(AmbientLight::Create({
        .color = 0xFFFFFF,
        .intensity = 0.3f
    }));

    auto point_light = PointLight::Create({
        .color = 0xFFFFFF,
        .intensity = 1.0f,
        .attenuation = {
            .base = 1.0f,
            .linear = 0.0f,
            .quadratic = 0.0f
        }
    });
    point_light->transform.Translate({2.0f, 2.0f, 4.0f});
    Add(point_light);

    auto geometry = BoxGeometry::Create(box_params_);
    auto base_material = PhongMaterial::Create(0x049EF4);
    base_material->polygon_offset_factor = 1.0f;
    base_material->polygon_offset_units = 1.0f;

    mesh_ = Mesh::Create(geometry, base_material);
    Add(mesh_);

    auto wireframe_geometry = WireframeGeometry::Create(geometry.get());
    auto wireframe_material = UnlitMaterial::Create();
    wireframe_material->fog = false;
    wireframes_ = Mesh::Create(wireframe_geometry, wireframe_material);
    mesh_->Add(wireframes_);
}

auto ExamplePrimitives::OnAttached(SharedContextPointer context) -> void {
    Add(OrbitControls::Create(context->camera, {
        .radius = 5.0f,
        .pitch = math::DegToRad(25.0f),
        .yaw = math::DegToRad(45.0f)
    }));
}

auto ExamplePrimitives::ContextMenu() -> void {
    static bool dirty = false;

    static auto primitives = std::array<const char*, 5> {
        "box", "cone", "cylinder", "plane", "sphere"
    };

    UIDropDown("primitive", primitives, curr_primitive_,
        [this](std::string_view str) {
            curr_primitive_ = str;
            dirty = true;
    });

    if (curr_primitive_ == "box") {
        BoxContextMenu(dirty);
    } else if (curr_primitive_ == "cone") {
        ConeContextMenu(dirty);
    } else if (curr_primitive_ == "cylinder") {
        CylinderContextMenu(dirty);
    } else if (curr_primitive_ == "plane") {
        PlaneContextMenu(dirty);
    } else if (curr_primitive_ == "sphere") {
        SphereContextMenu(dirty);
    }

    if (dirty) {
        dirty = false;
        std::shared_ptr<Geometry> geometry;

        if (curr_primitive_ == "box") {
            geometry = BoxGeometry::Create(box_params_);
        } else if (curr_primitive_ == "cone") {
            geometry = ConeGeometry::Create(cone_params_);
        } else if (curr_primitive_ == "cylinder") {
            geometry = CylinderGeometry::Create(cylinder_params_);
        } else if (curr_primitive_ == "plane") {
            geometry = PlaneGeometry::Create(plane_params_);
        } else if (curr_primitive_ == "sphere") {
            geometry = SphereGeometry::Create(sphere_params_);
        }

        mesh_->SetGeometry(geometry);
        wireframes_->SetGeometry(WireframeGeometry::Create(geometry.get()));
    }
}

auto ExamplePrimitives::InitializeParams() -> void {
    cone_params_.radius = 0.5f;
    cylinder_params_.radius_top = 0.4f;
    cylinder_params_.radius_bottom = 0.4f;
    sphere_params_.radius = 0.8f;
}

auto ExamplePrimitives::BoxContextMenu(bool& dirty) -> void {
    UISliderFloat("width", box_params_.width, 1.0f, 5.0f, dirty);
    UISliderFloat("height", box_params_.height, 1.0f, 5.0f, dirty);
    UISliderFloat("depth", box_params_.depth, 1.0f, 5.0f, dirty);
    UISliderUnsigned("width_segments", box_params_.width_segments, 1, 20, dirty);
    UISliderUnsigned("height_segments", box_params_.height_segments, 1, 20, dirty);
    UISliderUnsigned("depth_segments", box_params_.depth_segments, 1, 20, dirty);
}

auto ExamplePrimitives::ConeContextMenu(bool& dirty) -> void {
    UISliderFloat("radius", cone_params_.radius, 0.0f, 1.0f, dirty);
    UISliderFloat("height", cone_params_.height, 0.1f, 5.0f, dirty);
    UISliderUnsigned("radial_segments", cone_params_.radial_segments, 3, 64, dirty);
    UISliderUnsigned("height_segments", cone_params_.height_segments, 1, 20, dirty);
    UICheckbox("open_ended", cone_params_.open_ended, dirty);
}

auto ExamplePrimitives::CylinderContextMenu(bool& dirty) -> void {
    UISliderFloat("radius_top", cylinder_params_.radius_top, 0.0f, 1.0f, dirty);
    UISliderFloat("radius_bottom", cylinder_params_.radius_bottom, 0.0f, 1.0f, dirty);
    UISliderFloat("height", cylinder_params_.height, 1.0f, 5.0f, dirty);
    UISliderUnsigned("radial_segments", cylinder_params_.radial_segments, 3, 64, dirty);
    UISliderUnsigned("height_segments", cylinder_params_.height_segments, 1, 20, dirty);
    UICheckbox("open_ended", cylinder_params_.open_ended, dirty);
}

auto ExamplePrimitives::PlaneContextMenu(bool& dirty) -> void {
    UISliderFloat("width", plane_params_.width, 1.0f, 5.0f, dirty);
    UISliderFloat("height", plane_params_.height, 1.0f, 5.0f, dirty);
    UISliderUnsigned("width_segments", plane_params_.width_segments, 1, 20, dirty);
    UISliderUnsigned("height_segments", plane_params_.height_segments, 1, 20, dirty);
}

auto ExamplePrimitives::SphereContextMenu(bool& dirty) -> void {
    UISliderFloat("radius", sphere_params_.radius, 0.5f, 2.0f, dirty);
    UISliderUnsigned("width_segments", sphere_params_.width_segments, 3, 64, dirty);
    UISliderUnsigned("height_segments", sphere_params_.height_segments, 2, 64, dirty);
}