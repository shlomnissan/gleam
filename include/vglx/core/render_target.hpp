/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/core/disposable.hpp"
#include "vglx/core/identity.hpp"
#include "vglx/textures/texture.hpp"

#include <memory>

namespace vglx {

class VGLX_EXPORT RenderTarget : public Disposable, public Identity {
public:
    struct Parameters {
        int width;
        int height;
        Texture::Format format;
        bool has_depth;
        bool enable_reads {false};
    };

    const int width;

    const int height;

    const Texture::Format format;

    const bool has_depth;

    unsigned int renderer_id;

    explicit RenderTarget(const Parameters& params);

    [[nodiscard]] static auto Create(const Parameters& params)
      -> std::shared_ptr<RenderTarget> {
        return std::make_shared<RenderTarget>(params);
    }

    ~RenderTarget() override;
};

}