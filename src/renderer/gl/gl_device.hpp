/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/core/renderer.hpp"

namespace vglx::gl {

auto limits() -> const Renderer::Limits&;

auto driver_info() -> const Renderer::DriverInfo&;

}
