/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/geometries/buffer_attribute.hpp"

#include "utilities/assert.hpp"
#include "utilities/logger.hpp"

namespace vglx {

namespace {

auto get_components_from_format(BufferAttribute::Format format) -> uint32_t {
    using enum BufferAttribute::Format;
    switch (format) {
        case Float32x1: return 1;
        case Float32x2: return 2;
        case Float32x3: return 3;
        case Float32x4: return 4;
        case Float32x16: return 16;
        default: VGLX_UNREACHABLE();
    }
}

}

BufferAttribute::BufferAttribute(const Params& params, std::vector<float> data)
    : name {params.name},
      format {params.format},
      rate {params.rate},
      data_ {std::move(data)}
{
    if (name.empty()) {
        Logger::Log(LogLevel::Error, "Buffer attribute initialization missing a name");
    }

    if (data_.size() % Components() != 0) {
        Logger::Log(LogLevel::Error, "Buffer attribute data is not divisible by number of components");
    }
}

auto BufferAttribute::SetData(std::vector<float> data) -> void {
    if (data.size() % Components() != 0) {
        Logger::Log(LogLevel::Error, "Buffer attribute skipped data update. Data is not divisible by number of components");
        return;
    }

    data_ = std::move(data);
    version_++;
}

auto BufferAttribute::Components() const -> uint32_t {
    return get_components_from_format(format);
}

auto BufferAttribute::ElementCount() const -> uint32_t {
    return static_cast<uint32_t>(data_.size()) / get_components_from_format(format);
}

BufferAttribute::~BufferAttribute() {
    Dispose();
}

}
