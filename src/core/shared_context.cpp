/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/core/shared_context.hpp"

#include "vglx/core/window.hpp"

namespace vglx {

auto SharedContext::Create(const Window* window, Camera* camera, LoadScheduler* scheduler)
  -> std::unique_ptr<SharedContext> {
    return std::make_unique<SharedContext> (
        camera,
        window->AspectRatio(),
        window->FramebufferWidth(),
        window->FramebufferHeight(),
        window->Width(),
        window->Height(),
        std::make_unique<TextureLoaderXYZ>(scheduler),
        std::make_unique<MeshLoaderXYZ>(scheduler)
    );
}

}