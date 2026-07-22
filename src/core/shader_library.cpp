/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "core/shader_library.hpp"

#include "vglx/materials/pbr_material.hpp"
#include "vglx/materials/phong_material.hpp"
#include "vglx/materials/shader_material.hpp"
#include "vglx/materials/sprite_material.hpp"
#include "vglx/materials/unlit_material.hpp"

#include "utilities/logger.hpp"

#include "shaders/headers/pbr_material_frag.h"
#include "shaders/headers/pbr_material_vert.h"
#include "shaders/headers/phong_material_frag.h"
#include "shaders/headers/phong_material_vert.h"
#include "shaders/headers/sprite_material_frag.h"
#include "shaders/headers/sprite_material_vert.h"
#include "shaders/headers/unlit_material_frag.h"
#include "shaders/headers/unlit_material_vert.h"
#include "shaders/snippets/headers/frag_global_lights_glsl.h"
#include "shaders/snippets/headers/frag_global_fog_glsl.h"
#include "shaders/snippets/headers/frag_global_params_glsl.h"
#include "shaders/snippets/headers/vert_global_params_glsl.h"
#include "shaders/snippets/headers/vert_main_varyings_glsl.h"

#include <unordered_map>

namespace vglx {

auto ShaderLibrary::GetShaderSource(const ProgramAttributes& attrs) const -> std::vector<ShaderInfo> {
    if (attrs.type == Material::Type::PBRMaterial) {
        return {{
            ShaderType::kVertexShader,
            ProcessShader(attrs, _SHADER_pbr_material_vert)
        }, {
            ShaderType::kFragmentShader,
            ProcessShader(attrs, _SHADER_pbr_material_frag)
        }};
    }

    if (attrs.type == Material::Type::PhongMaterial) {
        return {{
            ShaderType::kVertexShader,
            ProcessShader(attrs, _SHADER_phong_material_vert)
        }, {
            ShaderType::kFragmentShader,
            ProcessShader(attrs, _SHADER_phong_material_frag)
        }};
    }

    if (attrs.type == Material::Type::ShaderMaterial) {
        return {{
            ShaderType::kVertexShader,
            ProcessShader(attrs, attrs.vertex_shader)
        }, {
            ShaderType::kFragmentShader,
            ProcessShader(attrs, attrs.fragment_shader)
        }};
    }

    if (attrs.type == Material::Type::SpriteMaterial) {
        return {{
            ShaderType::kVertexShader,
            ProcessShader(attrs, _SHADER_sprite_material_vert)
        }, {
            ShaderType::kFragmentShader,
            ProcessShader(attrs, _SHADER_sprite_material_frag)
        }};
    }

    if (attrs.type == Material::Type::UnlitMaterial) {
        return {{
            ShaderType::kVertexShader,
            ProcessShader(attrs, _SHADER_unlit_material_vert)
        }, {
            ShaderType::kFragmentShader,
            ProcessShader(attrs, _SHADER_unlit_material_frag)
        }};
    }

    Logger::Log(
        LogLevel::Error,
        "Shader source not found for unknown material {}_material",
        Material::TypeToString(attrs.type)
    );

    return {};
}

auto ShaderLibrary::ProcessShader(
    const ProgramAttributes& attrs,
    std::string_view source
) const -> std::string {
    auto output = std::string {source};
    InjectAttributes(attrs, output);
    ResolveIncludes(output);
    return output;
}

auto ShaderLibrary::InjectAttributes(
    const ProgramAttributes& attrs,
    std::string& source
) const -> void {
    auto features = std::string {};

    if (attrs.color) features += "#define USE_COLOR\n";
    if (attrs.flat_shaded) features += "#define USE_FLAT_SHADED\n";
    if (attrs.fog) features += "#define USE_FOG\n";
    if (attrs.ibl) features += "#define USE_IBL\n";
    if (attrs.instancing) features += "#define USE_INSTANCING\n";
    if (attrs.flip_normals) features += "#define USE_FLIP_NORMALS\n";
    if (attrs.vertex_color) features += "#define USE_VERTEX_COLOR\n";
    if (attrs.shadow_maps) features += "#define USE_SHADOW_MAPS\n";
    if (attrs.pcf_shadows) features += "#define USE_PCF_SHADOWS\n";
    if (attrs.point_shadow_maps) features += "#define USE_POINT_SHADOW_MAPS\n";

    if (attrs.albedo_map) features += "#define USE_ALBEDO_MAP\n";
    if (attrs.alpha_map) features += "#define USE_ALPHA_MAP\n";
    if (attrs.ao_map) features += "#define USE_AO_MAP\n";
    if (attrs.emissive_map) features += "#define USE_EMISSIVE_MAP\n";
    if (attrs.environment_map) features += "#define USE_ENVIRONMENT_MAP\n";
    if (attrs.metallic_map) features += "#define USE_METALLIC_MAP\n";
    if (attrs.normal_map && attrs.tangent) features += "#define USE_NORMAL_MAP\n";
    if (attrs.roughness_map) features += "#define USE_ROUGHNESS_MAP\n";
    if (attrs.size_attenuation) features += "#define USE_SIZE_ATTENUATION\n";
    if (attrs.specular_map) features += "#define USE_SPECULAR_MAP\n";
    if (attrs.texture_map) features += "#define USE_TEXTURE_MAP\n";

    const auto lights = attrs.num_lights;
    features += "#define NUM_LIGHTS " + std::to_string(lights) + '\n';

    const auto token = std::string_view {"#pragma inject_attributes"};
    const auto pos = source.find(token);
    if (pos == std::string::npos) {
        Logger::Log(
            LogLevel::Error,
            "The '#pragma inject_attributes' token is missing in program {}",
            Material::TypeToString(attrs.type)
        );
        return;
    }

    source.replace(pos, token.size(), features);
}

auto ShaderLibrary::ResolveIncludes(std::string& source) const -> void {
    static const std::unordered_map<std::string, std::string> include_map = {
        {"snippets/frag_global_lights.glsl", _SNIPPET_frag_global_lights},
        {"snippets/frag_global_fog.glsl", _SNIPPET_frag_global_fog},
        {"snippets/frag_global_params.glsl", _SNIPPET_frag_global_params},
        {"snippets/vert_global_params.glsl", _SNIPPET_vert_global_params},
        {"snippets/vert_main_varyings.glsl", _SNIPPET_vert_main_varyings}
    };

    for (const auto& [include, content] : include_map) {
        auto token = std::string {"#include \"" + include + "\""};
        auto pos = source.find(token);
        if (pos != std::string::npos) {
            source.replace(pos, token.size(), content);
        }
    }
}

}
