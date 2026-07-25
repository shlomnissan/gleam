#version 410 core

#extension GL_GOOGLE_include_directive : enable

#pragma inject_attributes

#include "snippets/frag_global_params.glsl"

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_AlphaMap;

void main() {
    #ifdef USE_ALPHA_TEST
        float opacity = u_Opacity;

        #ifdef USE_ALBEDO_MAP
            opacity *= texture(u_AlbedoMap, v_TexCoords).a;
        #endif

        #ifdef USE_ALPHA_MAP
            opacity *= texture(u_AlphaMap, v_TexCoords).r;
        #endif

        if (opacity < u_AlphaTest) discard;
    #endif
}
