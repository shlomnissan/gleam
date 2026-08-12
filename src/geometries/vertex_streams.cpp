/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "geometries/vertex_streams.hpp"

#include "vglx/geometries/buffer_attribute.hpp"

#include <utility>

namespace vglx {

auto VertexStreams::AddTo(Geometry& geometry) -> void {
    if (!positions.empty()) {
        geometry.AddAttribute(BufferAttribute::Create({
            .name = BufferAttribute::kPosition,
            .format = BufferAttribute::Format::Float32x3,
            .rate = BufferAttribute::Rate::Vertex
        }, std::move(positions)));
    }

    if (!normals.empty()) {
        geometry.AddAttribute(BufferAttribute::Create({
            .name = BufferAttribute::kNormal,
            .format = BufferAttribute::Format::Float32x3,
            .rate = BufferAttribute::Rate::Vertex
        }, std::move(normals)));
    }

    if (!uvs.empty()) {
        geometry.AddAttribute(BufferAttribute::Create({
            .name = BufferAttribute::kTexCoord,
            .format = BufferAttribute::Format::Float32x2,
            .rate = BufferAttribute::Rate::Vertex
        }, std::move(uvs)));
    }

    if (!tangents.empty()) {
        geometry.AddAttribute(BufferAttribute::Create({
            .name = BufferAttribute::kTangent,
            .format = BufferAttribute::Format::Float32x4,
            .rate = BufferAttribute::Rate::Vertex
        }, std::move(tangents)));
    }

    if (!colors.empty()) {
        geometry.AddAttribute(BufferAttribute::Create({
            .name = BufferAttribute::kColor,
            .format = BufferAttribute::Format::Float32x3,
            .rate = BufferAttribute::Rate::Vertex
        }, std::move(colors)));
    }

    if (!indices.empty()) {
        geometry.SetIndices(std::move(indices));
    }
}

}
