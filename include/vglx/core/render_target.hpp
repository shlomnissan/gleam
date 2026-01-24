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

#include <cstdint>
#include <memory>
#include <span>
#include <vector>

namespace vglx {

class VGLX_EXPORT RenderTarget : public Disposable, public Identity {
public:
    struct Parameters {
        int width;
        int height;
        Texture::Format format;
        bool has_depth;
        bool enable_readback {false};
    };

    const int width;

    const int height;

    const Texture::Format format;

    const bool has_depth;

    const bool enable_readback;

    unsigned int renderer_id;

    explicit RenderTarget(const Parameters& params);

    [[nodiscard]] static auto Create(const Parameters& params)
      -> std::shared_ptr<RenderTarget> {
        return std::make_shared<RenderTarget>(params);
    }

    [[nodiscard]] auto ReadColorData() const -> std::span<const std::uint8_t>;

    ~RenderTarget() override;

private:
    friend class GLFramebuffers;

    std::vector<uint8_t> color_data_;

    bool has_readback_ {false};
};

}