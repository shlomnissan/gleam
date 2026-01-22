/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/core/render_target.hpp"

#include "vglx/textures/texture.hpp"

namespace vglx {

RenderTarget::RenderTarget(const Parameters& params)
  : width(params.width),
    height(params.height),
    has_depth(params.has_depth) {}

RenderTarget::~RenderTarget() {
    Disposable::Dispose();
}

}