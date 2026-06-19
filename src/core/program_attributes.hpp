/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/scene/renderable.hpp"
#include "vglx/scene/scene.hpp"

#include <cstdint>
#include <string_view>
#include <utility>

namespace vglx {

struct ProgramAttributes {
    struct LightsCounter {
        uint8_t directional {0};
        uint8_t point {0};
        uint8_t spot {0};
    };

    std::size_t key {0};

    Material::Type type;

    // Used to store shader source code for shader materials
    std::string_view vertex_shader;
    std::string_view fragment_shader;

    uint8_t num_lights {0};

    bool color {false};
    bool flat_shaded {false};
    bool fog {false};
    bool ibl {false};
    bool instancing {false};
    bool tangent {false};
    bool two_sided {false};
    bool vertex_color {false};

    bool albedo_map {false};
    bool alpha_map {false};
    bool ao_map {false};
    bool emissive_map {false};
    bool environment_map {false};
    bool metallic_map {false};
    bool normal_map {false};
    bool roughness_map {false};
    bool size_attenuation {false};
    bool specular_map {false};
    bool texture_map {false};

    ProgramAttributes(
        Renderable* renderable,
        const LightsCounter& lights,
        const Scene* scene
    );
};

}