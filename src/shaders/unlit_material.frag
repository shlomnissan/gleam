#version 410 core

#extension GL_GOOGLE_include_directive : enable

#pragma inject_attributes

layout (location = 0) out vec4 o_FragColor;

#ifdef USE_UV
    in vec2 v_TexCoords;
#endif

#ifdef USE_VERTEX_COLOR
    in vec3 v_Color;
#endif

#ifdef USE_INSTANCING
    in vec3 v_InstanceColor;
#endif

#ifdef USE_FOG
    in float v_ViewDepth;
#endif

uniform float u_Opacity;
uniform vec3 u_Color;

#ifdef USE_ALPHA_TEST
    uniform float u_AlphaTest;
#endif

#include "include/fog.incl.glsl"

uniform sampler2D u_AlphaMap;
uniform sampler2D u_TextureMap;

void main() {
    vec3 output_color = u_Color;
    float opacity = u_Opacity;

    #ifdef USE_INSTANCING
        output_color *= v_InstanceColor;
    #endif

    #ifdef USE_VERTEX_COLOR
        output_color *= v_Color;
    #endif

    #ifdef USE_TEXTURE_MAP
        output_color *= texture(u_TextureMap, v_TexCoords).rgb;
        opacity *= texture(u_TextureMap, v_TexCoords).a;
    #endif

    #ifdef USE_ALPHA_MAP
        vec4 alpha_sample = texture(u_AlphaMap, v_TexCoords);
        opacity *= alpha_sample.r;
    #endif

    #ifdef USE_ALPHA_TEST
        if (opacity < u_AlphaTest) discard;
    #endif

    #ifdef USE_FOG
        applyFog(output_color, v_ViewDepth);
    #endif

    o_FragColor = vec4(output_color, opacity);
}
