/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/textures/dynamic_texture_2d.hpp"

namespace vglx {

auto DynamicTexture2D::UpdateSubregion(
    unsigned int mip_level,
    int x,
    int y,
    int w,
    int h,
    std::span<const std::uint8_t> data
) -> void {
    // TODO: implement.
}

}