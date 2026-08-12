/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/geometries/geometry.hpp"

#include <cstdint>
#include <vector>

namespace vglx {

struct VertexStreams {
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> uvs;
    std::vector<float> tangents;
    std::vector<float> colors;
    std::vector<uint32_t> indices;

    auto AddTo(Geometry& geometry) -> void;
};

}
