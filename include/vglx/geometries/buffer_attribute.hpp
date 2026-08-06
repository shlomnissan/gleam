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

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace vglx {

class VGLX_EXPORT BufferAttribute : public Disposable, public Identity {
public:
    static constexpr std::string_view kPosition = "a_Position";
    static constexpr std::string_view kNormal = "a_Normal";
    static constexpr std::string_view kTexCoord = "a_TexCoord";
    static constexpr std::string_view kColor = "a_Color";
    static constexpr std::string_view kTangent = "a_Tangent";
    static constexpr std::string_view kInstanceTransform = "a_InstanceTransform";
    static constexpr std::string_view kInstanceColor = "a_InstanceColor";

    enum class Format {
        Float32x1,
        Float32x2,
        Float32x3,
        Float32x4,
        Float32x16
    };

    enum class Rate {
        Vertex,
        Instance
    };

    struct Params {
        std::string_view name {};
        Format format {Format::Float32x1};
        Rate rate {Rate::Vertex};
    };

    BufferAttribute(const Params& params, std::vector<float> data);

    // delete copy constructor and assignment operator
    BufferAttribute(const BufferAttribute&) = delete;
    auto operator=(const BufferAttribute&) -> BufferAttribute& = delete;

    [[nodiscard]] static auto Create(const Params& params, std::vector<float> data) -> std::shared_ptr<BufferAttribute> {
        return std::make_shared<BufferAttribute>(params, std::move(data));
    }

    auto SetData(std::vector<float> data) -> void;

    [[nodiscard]] auto Components() const -> uint32_t;

    [[nodiscard]] auto ElementCount() const -> uint32_t;

    [[nodiscard]] auto IsValid() const -> bool { return !name_.empty() && !data_.empty(); }

    [[nodiscard]] auto GetData() const -> const std::vector<float>& { return data_; }

    [[nodiscard]] auto GetFormat() const -> Format { return format_; }

    [[nodiscard]] auto GetName() const -> const std::string& { return name_; }

    [[nodiscard]] auto GetRate() const -> Rate { return rate_; }

    [[nodiscard]] auto GetVersion() const -> uint32_t { return version_; }

private:
    std::string name_ {};
    std::vector<float> data_;

    uint32_t version_ {0};

    Format format_ {Format::Float32x1};
    Rate rate_ {Rate::Vertex};
};

}
