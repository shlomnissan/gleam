/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/scene/sprite.hpp"

namespace vglx {

Sprite::Sprite(std::shared_ptr<SpriteMaterial> material)
  : geometry_(SharedGeometry()), material_(material) {
    if (material_ == nullptr) {
        material_ = SpriteMaterial::Create(nullptr);
    }
}

auto Sprite::SharedGeometry() -> std::shared_ptr<Geometry>& {
    static auto geometry = []() {
        auto g = Geometry::Create({
           -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
        }, {0, 1, 2, 0, 2, 3});

        g->SetAttribute({Geometry::VertexAttributeType::Position, 3});
        g->SetAttribute({Geometry::VertexAttributeType::UV, 2});

        return g;
    }();

    return geometry;
}

}