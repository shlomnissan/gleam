/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/lights/light.hpp"

#include "vglx/cameras/camera.hpp"
#include "vglx/math/color.hpp"
#include "vglx/math/matrix4.hpp"
#include "vglx/math/vector3.hpp"

#include "renderer/gl/gl_uniform_buffer.hpp"

#include <array>
#include <cstdint>

namespace vglx {

class GLLights {
public:
    static constexpr auto kMaxLights = 10;

    struct alignas(16) UniformLight {
        alignas(4) int type {0};
        alignas(16) Color color {0xFFFFFF};
        alignas(16) Vector3 position {Vector3::Zero()};
        alignas(16) Vector3 direction {Vector3::Zero()};
        alignas(4) float cone_cos {0.0f};
        alignas(4) float penumbra_cos {0.0f};
        alignas(4) float range {0.0f};
        alignas(4) int shadow_layer_index {-1};
        alignas(4) float shadow_bias {0.0f};
        alignas(4) float shadow_near {0.0f};
        alignas(4) float shadow_far {0.0f};
        alignas(16) Matrix4 shadow_transform {Matrix4::Identity()};
    };

    struct alignas(16) UniformLights {
        alignas(16) UniformLight lights[kMaxLights];
    };

    Color ambient_light {0x000000};

    uint8_t ambient {0};
    uint8_t directional {0};
    uint8_t point {0};
    uint8_t spot {0};

    bool has_shadow_casters {false};

    GLLights() = default;

    // delete copy constructor and assignment operator
    GLLights(const GLLights&) = delete;
    auto operator=(const GLLights&) -> GLLights& = delete;

    // delete move constructor and assignment operator
    GLLights(GLLights&&) = delete;
    auto operator=(GLLights&&) -> GLLights& = delete;

    auto AddLight(
        Light* light,
        Camera* camera,
        Matrix4* shadow_transform = nullptr,
        int shadow_layer_index = -1
    ) -> void;

    auto HasLights() const -> bool;

    auto Reset() -> void;

    auto Update() -> void;

private:
    UniformLights lights_ {};

    GLUniformBuffer uniform_buffer_ {"ub_Lights", sizeof(UniformLights)};

    unsigned int idx_ {0};
};

}
