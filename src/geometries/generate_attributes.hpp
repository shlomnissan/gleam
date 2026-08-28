/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <cstdint>
#include <vector>

namespace vglx {

[[nodiscard]] auto generate_normals(
    const std::vector<float>& positions,
    const std::vector<uint32_t>& indices
) -> std::vector<float>;

[[nodiscard]] auto generate_tangents(
    const std::vector<float>& positions,
    const std::vector<float>& normals,
    const std::vector<float>& uvs,
    const std::vector<uint32_t>& indices
) -> std::vector<float>;

}
