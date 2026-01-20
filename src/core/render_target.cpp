/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/core/render_target.hpp"

#include "vglx/textures/texture.hpp"

namespace vglx {

struct RenderTarget::Impl {
    unsigned int renderer_id;

    std::shared_ptr<Texture> color_attachment;
};

RenderTarget::RenderTarget(const Parameters& params)
  : width(params.width),
    height(params.height),
    has_depth(params.has_depth),
    enable_readback(params.enable_readback),
    impl_(std::make_unique<RenderTarget::Impl>()) {}

}