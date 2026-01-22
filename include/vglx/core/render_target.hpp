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

#include <memory>

namespace vglx {

class VGLX_EXPORT RenderTarget : public Disposable, public Identity {
public:
    struct Parameters {
        int width {0};
        int height {0};
        bool has_depth {true};
    };

    const int width;

    const int height;

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