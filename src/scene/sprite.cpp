/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/scene/sprite.hpp"

#include "vglx/geometries/buffer_attribute.hpp"

namespace vglx {

Sprite::Sprite(std::shared_ptr<SpriteMaterial> material)
  : geometry_(SharedGeometry()), material_(material) {
    if (material_ == nullptr) {
        material_ = SpriteMaterial::Create();
    }
}

auto Sprite::SharedGeometry() -> std::shared_ptr<Geometry>& {
    static auto geometry = []() {
        auto g = Geometry::Create();

        g->AddAttribute(BufferAttribute::Create({
            .name = BufferAttribute::kPosition,
            .format = BufferAttribute::Format::Float32x3,
            .rate = BufferAttribute::Rate::Vertex
        }, {
           -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.5f,  0.5f, 0.0f,
           -0.5f,  0.5f, 0.0f
        }));

        g->AddAttribute(BufferAttribute::Create({
            .name = BufferAttribute::kTexCoord,
            .format = BufferAttribute::Format::Float32x2,
            .rate = BufferAttribute::Rate::Vertex
        }, {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        }));

        g->SetIndices({0, 1, 2, 0, 2, 3});

        return g;
    }();

    return geometry;
}

}
