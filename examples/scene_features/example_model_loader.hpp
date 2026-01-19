/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <vglx/cameras.hpp>
#include <vglx/geometries.hpp>
#include <vglx/materials.hpp>
#include <vglx/scene.hpp>
#include <vglx/textures.hpp>

#include <memory>

#include "example_scene.hpp"

class ExampleModelLoader : public ExampleScene {
public:
    ExampleModelLoader();

    auto OnAttached(vglx::SharedContextPointer context) -> void override;

    auto OnUpdate(float delta) -> void override;

    auto ContextMenu() -> void override;

private:
    vglx::Mesh* sphere_ {nullptr};
    vglx::Node* model_ {nullptr};

    std::shared_ptr<vglx::Texture> albedo_map_;
    std::shared_ptr<vglx::Texture> normal_map_;
    std::shared_ptr<vglx::Texture> specular_map_;

    vglx::PhongMaterial* material_ {nullptr};

    bool show_albedo_map_ {true};
    bool show_normal_map_ {true};
    bool show_specular_map_ {true};
};