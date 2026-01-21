/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include <memory>

namespace vglx {

class VGLX_EXPORT RenderTarget {
public:
    struct Parameters {
        int width {0};
        int height {0};
        bool has_depth {true};
        bool enable_readback {false};
    };

    const int width;

    const int height;

    const bool has_depth;

    const bool enable_readback;

    explicit RenderTarget(const Parameters& params);

    [[nodiscard]] static auto Create(const Parameters& params)
      -> std::shared_ptr<RenderTarget> {
        return std::make_shared<RenderTarget>(params);
    }

    ~RenderTarget();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}