/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <vector>

#include <vglx/vglx.hpp>
#include <imgui/imgui.h>

#include "ui_helpers.hpp"
#include "example_scene.hpp"

#include "animation/example_animated_transform.hpp"
#include "lighting/example_directional_light.hpp"
#include "lighting/example_point_light.hpp"
#include "lighting/example_spot_light.hpp"
#include "materials/example_pbr_material.hpp"
#include "materials/example_phong_material.hpp"
#include "materials/example_shader_material.hpp"
#include "materials/example_unlit_material.hpp"
#include "rendering_effects/example_blending.hpp"
#include "rendering_effects/example_fog.hpp"
#include "sandbox/example_sandbox.hpp"
#include "scene_features/example_debug_visuals.hpp"
#include "scene_features/example_frustum_culling.hpp"
#include "scene_features/example_mesh_instancing.hpp"
#include "scene_features/example_model_loader.hpp"
#include "scene_features/example_primitives.hpp"
#include "scene_features/example_sprite.hpp"

namespace {

    const auto examples = std::vector {
    "- Materials",
    "PBR Material",
    "Unlit Material",
    "Phong Material",
    "Shader Material",
    "- Lights",
    "Directional Light",
    "Point Light",
    "Spot Light",
    "- Rendering Effects",
    "Transparency & Blending",
    "Fog Effect",
    "- Scene Features",
    "Frustum Culling",
    "Mesh Instancing",
    "Model Loader",
    "Primitives",
    "Sprite",
    "Debug Visuals",
    "- Animation",
    "Animated Transform"
};

}

using SceneChangeCallback = std::function<void(std::unique_ptr<vglx::Scene>)>;

class Examples {
public:
    ExampleScene* scene_ptr {nullptr};

    Examples(vglx::Camera* camera, SceneChangeCallback cb)
      : camera_ {camera}, scene_change_cb_ {std::move(cb)} {
        Theme();
    }

    auto Draw() -> void {
        const auto height = ImGui::GetIO().DisplaySize.y;

        ImGui::SetNextWindowSize({250, height - 20.0f});
        ImGui::SetNextWindowPos({10, 10});
        ImGui::Begin("VGLX", nullptr,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove
        );

        if (ImGui::CollapsingHeader("Examples", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::BeginListBox("##ListBox", {235, 404})) {
                for (auto i = 0; i < examples.size(); i++) {
                    const auto name = std::string_view {examples[i]};
                    if (name.starts_with("-")) {
                        UISeparator();
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4, 0.4, 0.4, 1));
                        ImGui::TextUnformatted(name.substr(2).data());
                        ImGui::PopStyleColor();
                        UISeparator();
                    } else if (
                        ImGui::Selectable(name.data(), current_scene_ == i)
                        && current_scene_ != i
                    ) {
                        current_scene_ = i;
                        scene_change_cb_(GetScene());
                    }
                }
                ImGui::EndListBox();
            }
        }

        if (scene_ptr != nullptr && scene_ptr->show_context_menu_) {
            if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
                scene_ptr->ContextMenu();
            }
        }

        ImGui::End();
    }

    auto GetScene() -> std::unique_ptr<ExampleScene> {
        auto scene = std::unique_ptr<ExampleScene> {nullptr};
        auto name = std::string_view {examples[current_scene_]};

        if (name == "PBR Material") scene = std::make_unique<ExamplePBRMaterial>(camera_);
        if (name == "Unlit Material") scene = std::make_unique<ExampleUnlitMaterial>(camera_);
        if (name == "Phong Material") scene = std::make_unique<ExamplePhongMaterial>(camera_);
        if (name == "Shader Material") scene = std::make_unique<ExampleShaderMaterial>(camera_);
        if (name == "Directional Light") scene = std::make_unique<ExampleDirectionalLight>(camera_);
        if (name == "Point Light") scene = std::make_unique<ExamplePointLight>(camera_);
        if (name == "Spot Light") scene = std::make_unique<ExampleSpotLight>(camera_);
        if (name == "Transparency & Blending") scene = std::make_unique<ExampleBlending>(camera_);
        if (name == "Fog Effect") scene = std::make_unique<ExampleFog>(camera_);
        if (name == "Frustum Culling") scene = std::make_unique<ExampleFrustumCulling>(camera_);
        if (name == "Mesh Instancing") scene = std::make_unique<ExampleMeshInstancing>(camera_);
        if (name == "Model Loader") scene = std::make_unique<ExampleModelLoader>(camera_);
        if (name == "Primitives") scene = std::make_unique<ExamplePrimitives>(camera_);
        if (name == "Sprite") scene = std::make_unique<ExampleSprite>(camera_);
        if (name == "Debug Visuals") scene = std::make_unique<ExampleDebugVisuals>(camera_);
        if (name == "Animated Transform") scene = std::make_unique<ExampleAnimatedTransform>(camera_);
        if (scene == nullptr) scene = std::make_unique<ExampleSandbox>(camera_);

        scene_ptr = scene.get();

        return scene;
    }

private:
    vglx::Camera* camera_ {nullptr};

    SceneChangeCallback scene_change_cb_;

    int current_scene_ = 1;
};