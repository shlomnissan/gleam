/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/scene/billboard.hpp"

#include "vglx/geometries/buffer_attribute.hpp"

#include <utility>

namespace vglx {

Billboard::Billboard(std::shared_ptr<BillboardMaterial> material)
  : material_(material) {
    if (material_ == nullptr) {
        material_ = BillboardMaterial::Create();
    }
}

auto Billboard::SharedGeometry() -> std::shared_ptr<Geometry>& {
    static auto geometry = std::shared_ptr<Geometry> {};

    // Rebuild on demand so disposing the shared geometry through a billboard
    // cannot permanently break billboard rendering.
    if (geometry == nullptr || geometry->Disposed()) {
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

        geometry = std::move(g);
    }

    return geometry;
}

}
