/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/core/shared_context.hpp"

#include "vglx/core/window.hpp"

namespace vglx {

auto SharedContext::Create(Window* window, Camera* camera)
  -> std::unique_ptr<SharedContext> {
    return std::make_unique<SharedContext> (
        camera,
        window->AspectRatio(),
        window->FramebufferWidth(),
        window->FramebufferHeight(),
        window->Width(),
        window->Height()
    );
}

}